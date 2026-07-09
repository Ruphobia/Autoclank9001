// SPDX-License-Identifier: GPL-3.0-or-later
#include "image_generator.hpp"

#include "../010_interface/status.hpp"
#include "../data/data.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <errno.h>
#include <filesystem>
#include <mutex>
#include <random>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// Every resident LLM has to leave both GPUs before sd-cli tries to
// cudaMalloc 9 GB for Chroma. Same cross-shutdown hooks the LLM roles
// use to boot each other off shared cards.
extern "C" void coder_shutdown_if_loaded();
extern "C" void qwen14b_shutdown_if_loaded();
extern "C" void physics_shutdown_if_loaded();
extern "C" void chemistry_shutdown_if_loaded();
extern "C" void vision_shutdown_if_loaded();
extern "C" void planner_shutdown_if_loaded();

namespace image_generator {
namespace {

void evict_all_llms() {
    // Fire each shutdown regardless of which are actually loaded; the
    // handles are idempotent when the model was never opened.
    coder_shutdown_if_loaded();
    qwen14b_shutdown_if_loaded();
    physics_shutdown_if_loaded();
    chemistry_shutdown_if_loaded();
    vision_shutdown_if_loaded();
    planner_shutdown_if_loaded();
}

namespace fs = std::filesystem;

// One generation at a time. Chroma at H=1024/W=1024 fills a P100 by
// itself; concurrent runs would trash VRAM. Serialise here so a caller
// waiting on generate() sees a clean queue.
std::mutex g_gen_mtx;

std::string env_or(const char * key, const std::string & fallback) {
    const char * v = std::getenv(key);
    return (v && *v) ? std::string(v) : fallback;
}

// The inputs sd-cli needs for a diffusion run. The layout differs by
// bundle kind:
//   "chroma"            - Chroma DiT + Flux VAE + T5-XXL text encoder
//                         (loaded via --t5xxl). mmproj unused.
//   "qwen_image_create" - Qwen-Image 20B + Qwen VAE + Qwen2.5-VL
//                         (loaded via --llm). No mmproj.
// text_encoder_mmproj + model_args stay empty for the create-only
// bundles above; the edit bundle uses them.
struct Paths {
    fs::path    sd_cli;
    fs::path    diffusion;
    fs::path    vae;
    fs::path    text_encoder;
    fs::path    text_encoder_mmproj;
    std::string kind;         // "chroma" | "qwen_image_create"
    std::string model_args;   // e.g. "chroma_use_dit_mask=false"
    std::string missing;      // empty when all mandatory files exist
};

// Precedence (highest wins):
//   1. SD_* env vars  (per-file debug overrides)
//   2. AC9_IMAGE_GEN_ROLE -> data::role_image_bundle_paths(role) (kind
//                            + file paths + model_args)
//   3. Hardcoded Chroma data/staging/ fallback
// Read env fresh every call so the Models settings tab's Save can hot-
// swap bundles at runtime by setenv()ing AC9_IMAGE_GEN_ROLE and letting
// the next generate() call re-resolve.
Paths resolve_paths() {
    Paths p;
    p.sd_cli = env_or("SD_CLI_BIN",
        "/home/jwoods/work/Autoclank9001/scratchpad/sdcpp_build/sd/build/bin/sd-cli");

    // Legacy Chroma defaults - what the operator ran before the Models
    // settings tab landed.
    std::string diffusion_default =
        "/home/jwoods/work/Autoclank9001/data/staging/Chroma1-HD-Q8_0.gguf";
    std::string vae_default =
        "/home/jwoods/work/Autoclank9001/data/staging/ae.safetensors";
    std::string encoder_default =
        "/home/jwoods/work/Autoclank9001/data/staging/t5-v1_1-xxl-encoder-Q8_0.gguf";
    std::string mmproj_default;         // Chroma has no mmproj partner.
    std::string kind_default = "chroma";
    std::string model_args_default = "chroma_use_dit_mask=false";

    if (const char * role = std::getenv("AC9_IMAGE_GEN_ROLE");
        role && *role) {
        if (auto bundle = data::role_image_bundle_paths(role)) {
            if (!bundle->kind.empty())            kind_default        = bundle->kind;
            if (!bundle->diffusion.empty())       diffusion_default   = bundle->diffusion.string();
            if (!bundle->vae.empty())             vae_default         = bundle->vae.string();
            if (!bundle->text_encoder.empty())    encoder_default     = bundle->text_encoder.string();
            if (!bundle->text_encoder_mmproj.empty())
                                                   mmproj_default      = bundle->text_encoder_mmproj.string();
            // model_args is bundle-defined even when empty (Qwen-Image
            // create wants no extra model-args); the bundle IS the
            // source of truth here, not a fallback.
            model_args_default = bundle->model_args;
        }
    }

    p.kind                = kind_default;
    p.diffusion           = env_or("SD_CHROMA_MODEL", diffusion_default);
    p.vae                 = env_or("SD_FLUX_VAE",     vae_default);
    p.text_encoder        = env_or("SD_T5XXL",        encoder_default);
    p.text_encoder_mmproj = mmproj_default;
    p.model_args          = model_args_default;

    // Mandatory-file existence checks. Labels track the bundle kind so
    // status() error messages read cleanly for whichever pipeline is
    // picked.
    const bool is_qwen = p.kind == "qwen_image_create";
    struct Check { const char * label; fs::path * path; };
    std::vector<Check> checks = {
        {"sd-cli",                                  &p.sd_cli},
        {is_qwen ? "Qwen-Image DiT" : "Chroma DiT", &p.diffusion},
        {is_qwen ? "Qwen VAE" : "Flux VAE",         &p.vae},
        {is_qwen ? "Qwen2.5-VL text encoder"
                 : "T5-XXL text encoder",           &p.text_encoder},
    };
    for (const auto & c : checks) {
        std::error_code ec;
        if (!fs::exists(*c.path, ec)) {
            if (!p.missing.empty()) p.missing += ", ";
            p.missing += c.label;
            p.missing += " (";
            p.missing += c.path->string();
            p.missing += ")";
        }
    }
    return p;
}

// Kebab-case a prompt into an on-disk-safe filename slug. Empty prompts
// map to "image" so we never mint a bare .png.
std::string slugify(const std::string & prompt, std::size_t max_len = 60) {
    std::string out;
    out.reserve(std::min(prompt.size(), max_len));
    bool last_dash = false;
    for (char c : prompt) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) {
            out.push_back(static_cast<char>(std::tolower(uc)));
            last_dash = false;
        } else if (!last_dash && !out.empty()) {
            out.push_back('-');
            last_dash = true;
        }
        if (out.size() >= max_len) break;
    }
    while (!out.empty() && out.back() == '-') out.pop_back();
    return out.empty() ? std::string("image") : out;
}

// ISO-8601 compact stamp: 20260707T034812Z. Appended so re-running the
// same prompt doesn't overwrite the previous PNG.
std::string time_stamp() {
    using namespace std::chrono;
    const auto  now = system_clock::now();
    std::time_t t   = system_clock::to_time_t(now);
    std::tm     tm{};
    gmtime_r(&t, &tm);
    char buf[80];
    std::snprintf(buf, sizeof(buf),
                  "%04d%02d%02dT%02d%02d%02dZ",
                  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(buf);
}

// Fork sd-cli with the given argv, capture its combined stdout+stderr
// into `log`, and return the wait() exit code. -1 means we failed to
// start it at all. Nothing is inherited from the parent shell (no
// PATH assumptions, no shell metacharacters).
int run_sd_cli(const std::vector<std::string> & argv, std::string & log) {
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        log = std::string("pipe() failed: ") + std::strerror(errno);
        return -1;
    }
    const pid_t pid = fork();
    if (pid < 0) {
        int e = errno;
        close(pipefd[0]); close(pipefd[1]);
        log = std::string("fork() failed: ") + std::strerror(e);
        return -1;
    }
    if (pid == 0) {
        // child: rewire both stdout + stderr to the pipe
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        std::vector<char *> raw;
        raw.reserve(argv.size() + 1);
        for (const auto & a : argv) raw.push_back(const_cast<char *>(a.c_str()));
        raw.push_back(nullptr);
        execv(argv[0].c_str(), raw.data());
        // If we get here, execv failed. Write a one-line marker for the
        // parent's log so the caller has a clue.
        std::fprintf(stderr, "execv failed: %s\n", std::strerror(errno));
        _exit(127);
    }
    // parent: drain the pipe until the child closes it.
    close(pipefd[1]);
    std::array<char, 4096> buf;
    while (true) {
        ssize_t n = read(pipefd[0], buf.data(), buf.size());
        if (n > 0) log.append(buf.data(), static_cast<std::size_t>(n));
        else if (n == 0) break;
        else if (errno == EINTR) continue;
        else break;
    }
    close(pipefd[0]);
    int wstatus = 0;
    while (waitpid(pid, &wstatus, 0) < 0 && errno == EINTR) {}
    if (WIFEXITED(wstatus))   return WEXITSTATUS(wstatus);
    if (WIFSIGNALED(wstatus)) return 128 + WTERMSIG(wstatus);
    return -1;
}

// Return the last `max_bytes` of `s` so the SSE layer stays small.
std::string tail(const std::string & s, std::size_t max_bytes = 4096) {
    if (s.size() <= max_bytes) return s;
    return "... (truncated) ...\n" + s.substr(s.size() - max_bytes);
}

// Roll a non-zero 63-bit seed. sd-cli accepts up to 2^63-1; we clamp
// there so the argv string always parses as a signed integer.
std::uint64_t pick_random_seed() {
    std::random_device                       rd;
    std::mt19937_64                          eng(rd());
    std::uniform_int_distribution<std::uint64_t> dist(1ULL,
        (1ULL << 63) - 1ULL);
    return dist(eng);
}

// If any LoRAs were requested, append `<lora:name:weight>` tokens to
// the prompt exactly the way sd-cli's --lora-model-dir + prompt-token
// path expects. Weights are formatted with a small fixed precision so
// they round-trip cleanly through argv.
std::string apply_lora_tokens(const std::string &                   prompt,
                              const std::vector<LoraRef> &          loras) {
    if (loras.empty()) return prompt;
    std::string out = prompt;
    for (const auto & l : loras) {
        if (l.name.empty()) continue;
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%.3f", l.weight);
        out += " <lora:";
        out += l.name;
        out += ":";
        out += buf;
        out += ">";
    }
    return out;
}

// The LoRA directory sd-cli scans for `<lora:name:weight>` tokens.
// Overridable via SD_LORA_DIR; defaults to data/staging/loras/ next to
// the Chroma model.
std::string lora_dir() {
    return env_or("SD_LORA_DIR",
        "/home/jwoods/work/Autoclank9001/data/staging/loras");
}

}  // anonymous namespace

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    const Paths p = resolve_paths();
    if (p.missing.empty()) {
        s.ready  = true;
        s.detail = "Chroma1-HD + Flux VAE + T5-XXL + sd-cli present; "
                   "ready to generate.";
    } else {
        s.ready  = false;
        s.detail = "image generator not ready - missing: " + p.missing;
    }
    return s;
}

Result generate(const std::string & prompt, const std::string & out_dir) {
    // Legacy signature. Forwards to the extended generate() with default
    // Options, preserving byte-for-byte the pre-consistency-plan behavior.
    return generate(prompt, out_dir, Options{});
}

Result generate(const std::string & prompt,
                const std::string & out_dir,
                const Options &     opts) {
    Result r;
    const Paths p = resolve_paths();
    if (!p.missing.empty()) {
        r.ok = false;
        r.message = "image generator not ready: " + p.missing;
        return r;
    }

    std::error_code ec;
    fs::create_directories(out_dir, ec);
    if (ec) {
        r.ok = false;
        r.message = "could not create output dir " + out_dir + ": " + ec.message();
        return r;
    }

    // Reject a bad init image early so callers see a clean failure Result
    // instead of a cryptic sd-cli parse error mid-log.
    if (!opts.init_img_path.empty() &&
        !fs::exists(opts.init_img_path, ec)) {
        r.ok = false;
        r.message = "init image not found: " + opts.init_img_path;
        return r;
    }

    // Resolve the seed. When the caller left seed=0 we roll a fresh one
    // and report it back in the Result so the caller can persist it
    // (canonical seed lock, per the research report's Level 1 recipe).
    const std::uint64_t seed =
        opts.seed != 0 ? opts.seed : pick_random_seed();
    r.seed = seed;

    // Apply LoRA tokens to the prompt when requested. Everything is
    // still passed as an argv entry - no shell, no injection surface.
    const std::string final_prompt = apply_lora_tokens(prompt, opts.lora_refs);

    const std::string fname = slugify(prompt) + "-" + time_stamp() + ".png";
    const fs::path    out   = fs::path(out_dir) / fname;

    // argv shape switches on bundle kind. Common args (input paths,
    // prompt, seed, output) are shared; sampler / cfg / model-args /
    // encoder-flag differ per family.
    std::vector<std::string> argv = {
        p.sd_cli.string(),
        "--diffusion-model", p.diffusion.string(),
        "--vae",             p.vae.string(),
        "-p",                final_prompt,
        "-H",                "1024",
        "-W",                "1024",
        "-s",                std::to_string(seed),
    };

    if (p.kind == "qwen_image_create") {
        // Qwen-Image 20B MMDiT (docs/qwen_image.md). Text conditioning
        // via Qwen2.5-VL loaded through --llm (single file; no mmproj
        // for create). Sampler + cfg + flow-shift match the tutorial
        // defaults. --offload-to-cpu keeps the 8 GB VL encoder off the
        // 16 GB card so the ~22 GB DiT has room. --diffusion-fa is the
        // sd-cli flag that turns on the fused attention path for the
        // Qwen-Image DiT.
        argv.push_back("--llm");             argv.push_back(p.text_encoder.string());
        argv.push_back("--cfg-scale");       argv.push_back("2.5");
        argv.push_back("--sampling-method"); argv.push_back("euler");
        argv.push_back("--steps");           argv.push_back("20");
        argv.push_back("--flow-shift");      argv.push_back("3");
        argv.push_back("--offload-to-cpu");
        argv.push_back("--diffusion-fa");
    } else {
        // Chroma family (default) - unchanged from the pre-refactor path
        // so operator behavior on chroma1-hd is byte-for-byte identical.
        argv.push_back("--t5xxl");           argv.push_back(p.text_encoder.string());
        argv.push_back("--cfg-scale");       argv.push_back("4.0");
        argv.push_back("--sampling-method"); argv.push_back("euler");
        argv.push_back("--steps");           argv.push_back("20");
        // --clip-on-cpu keeps T5 in RAM (leaves ~10 GB VRAM for Chroma).
        // --vae-tiling chunks the VAE decode so it doesn't need to
        // allocate a fresh 6.6 GB buffer on top of the 9 GB DiT.
        argv.push_back("--clip-on-cpu");
        argv.push_back("--vae-tiling");
    }
    if (!p.model_args.empty()) {
        argv.push_back("--model-args");
        argv.push_back(p.model_args);
    }

    // img2img: init image + strength. Only added when the caller supplied
    // an init image. Qwen-Image (create) also accepts -i for img2img
    // conditioning; the flag is family-agnostic in sd-cli.
    if (!opts.init_img_path.empty()) {
        argv.push_back("-i");
        argv.push_back(opts.init_img_path);
        double strength = opts.strength;
        if (strength < 0.0) strength = 0.0;
        if (strength > 1.0) strength = 1.0;
        char sbuf[32];
        std::snprintf(sbuf, sizeof(sbuf), "%.3f", strength);
        argv.push_back("--strength");
        argv.push_back(sbuf);
    }

    if (!opts.lora_refs.empty()) {
        argv.push_back("--lora-model-dir");
        argv.push_back(lora_dir());
    }

    argv.push_back("-o");
    argv.push_back(out.string());

    std::string log;
    // Serialize: two Chromas at once would OOM.
    std::lock_guard<std::mutex> lk(g_gen_mtx);
    // Kick every resident LLM off its GPU so Chroma can allocate its
    // 9 GB. Without this, qwen35 (~25 GB layer-split) or the legacy
    // qwen14b (~15 GB on card 0) fill VRAM and sd-cli OOMs during
    // "generating image: 1/1".
    evict_all_llms();
    // Keep the chat SSE heartbeat alive across the ~7-minute sd-cli
    // run. sd-cli is a separate process, so ac9's per-token
    // status::pulse() calls stop firing for the duration and the
    // rainbow "thinking" animation freezes. The no-role PulseScope
    // just ticks pulse() every ~500 ms in a background thread; it
    // does NOT set loading_set(), so the heartbeat payload carries
    // no role/loading fields and applyHeartbeatProgress on the
    // client skips its "loading (X)" rewrite. The layer event
    // ("image_gen") already established the headline as
    // "thinking (Chroma1-HD)" via layerToRole, and that stays put
    // for the whole subprocess.
    status::PulseScope _hb;
    int rc = run_sd_cli(argv, log);
    r.log_tail = tail(log);
    if (rc != 0) {
        r.ok      = false;
        r.message = "sd-cli exited " + std::to_string(rc);
        return r;
    }
    if (!fs::exists(out, ec)) {
        r.ok      = false;
        r.message = "sd-cli reported success but no PNG at " + out.string();
        return r;
    }
    r.ok         = true;
    r.image_path = out.string();
    r.message    = "generated " + out.string();
    return r;
}

}
