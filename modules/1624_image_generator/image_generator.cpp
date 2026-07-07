// SPDX-License-Identifier: GPL-3.0-or-later
#include "image_generator.hpp"

#include "../010_interface/status.hpp"

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

// The four inputs sd-cli needs for a Chroma run. All paths are checked
// against the filesystem at status() time so a missing piece surfaces
// as ready=false with an explanatory detail.
struct Paths {
    fs::path sd_cli;
    fs::path chroma;
    fs::path vae;
    fs::path t5xxl;
    std::string missing;   // empty when all four exist
};

Paths resolve_paths() {
    Paths p;
    // sd-cli lives under scratchpad/ until we settle on a permanent home
    // beside ac9. Override with SD_CLI_BIN if a repo binary lands later.
    p.sd_cli = env_or("SD_CLI_BIN",
        "/home/jwoods/work/Autoclank9001/scratchpad/sdcpp_build/sd/build/bin/sd-cli");
    // The three model files were downloaded into data/staging/.
    p.chroma = env_or("SD_CHROMA_MODEL",
        "/home/jwoods/work/Autoclank9001/data/staging/Chroma1-HD-Q8_0.gguf");
    p.vae    = env_or("SD_FLUX_VAE",
        "/home/jwoods/work/Autoclank9001/data/staging/ae.safetensors");
    p.t5xxl  = env_or("SD_T5XXL",
        "/home/jwoods/work/Autoclank9001/data/staging/t5-v1_1-xxl-encoder-Q8_0.gguf");
    for (const auto & pr : {std::pair{"sd-cli", &p.sd_cli},
                            std::pair{"Chroma model", &p.chroma},
                            std::pair{"Flux VAE",     &p.vae},
                            std::pair{"T5-XXL",       &p.t5xxl}}) {
        std::error_code ec;
        if (!fs::exists(*pr.second, ec)) {
            if (!p.missing.empty()) p.missing += ", ";
            p.missing += pr.first;
            p.missing += " (";
            p.missing += pr.second->string();
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
        s.detail = "image generator not ready — missing: " + p.missing;
    }
    return s;
}

Result generate(const std::string & prompt, const std::string & out_dir) {
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

    const std::string fname = slugify(prompt) + "-" + time_stamp() + ".png";
    const fs::path    out   = fs::path(out_dir) / fname;

    // Chroma flags mirror docs/chroma.md exactly, plus --output for
    // the file path and dimensions we want. Everything is passed as
    // an argv entry — no shell, no injection surface.
    std::vector<std::string> argv = {
        p.sd_cli.string(),
        "--diffusion-model", p.chroma.string(),
        "--vae",             p.vae.string(),
        "--t5xxl",           p.t5xxl.string(),
        "-p",                prompt,
        "--cfg-scale",       "4.0",
        "--sampling-method", "euler",
        "--steps",           "20",
        "-H",                "1024",
        "-W",                "1024",
        "--model-args",      "chroma_use_dit_mask=false",
        // --clip-on-cpu keeps the T5 text encoder in RAM (leaves ~10 GB
        // VRAM for Chroma). --vae-tiling chunks the VAE decode so it
        // doesn't need to allocate a fresh 6.6 GB buffer on top of the
        // 9 GB the Chroma DiT is already holding.
        "--clip-on-cpu",
        "--vae-tiling",
        "-o",                out.string(),
    };

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
