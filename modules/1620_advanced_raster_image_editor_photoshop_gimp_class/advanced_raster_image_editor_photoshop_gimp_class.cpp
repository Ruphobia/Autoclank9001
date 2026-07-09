// SPDX-License-Identifier: GPL-3.0-or-later
#include "advanced_raster_image_editor_photoshop_gimp_class.hpp"

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
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

extern "C" void coder_shutdown_if_loaded();
extern "C" void qwen14b_shutdown_if_loaded();
extern "C" void physics_shutdown_if_loaded();
extern "C" void chemistry_shutdown_if_loaded();
extern "C" void vision_shutdown_if_loaded();
extern "C" void planner_shutdown_if_loaded();

namespace advanced_raster_image_editor_photoshop_gimp_class {
namespace {

namespace fs = std::filesystem;

// Serialised for the same VRAM reasons as image_generator.
std::mutex g_edit_mtx;

void evict_all_llms() {
    coder_shutdown_if_loaded();
    qwen14b_shutdown_if_loaded();
    physics_shutdown_if_loaded();
    chemistry_shutdown_if_loaded();
    vision_shutdown_if_loaded();
    planner_shutdown_if_loaded();
}

std::string env_or(const char * key, const std::string & fallback) {
    const char * v = std::getenv(key);
    return (v && *v) ? std::string(v) : fallback;
}

// Editor argv shape switches on bundle kind:
//   "chroma"           - Chroma img2img at strength 0.95 (legacy pre-2511
//                        path; loses identity but produces real edits).
//   "qwen_image_edit"  - Qwen-Image-Edit-2511, dual encoder identity
//                        lock via --llm + --llm_vision, edit target
//                        supplied via -r, no --strength (edit is a
//                        native flow, not img2img blend). Requires
//                        --model-args qwen_image_zero_cond_t=true or
//                        the VAE returns solid white (documented in
//                        leejet/stable-diffusion.cpp docs/qwen_image_edit.md).
struct Paths {
    fs::path    sd_cli;
    fs::path    diffusion;
    fs::path    vae;
    fs::path    text_encoder;
    fs::path    text_encoder_mmproj;
    std::string kind;         // "chroma" | "qwen_image_edit"
    std::string model_args;
    std::string missing;
};

// Precedence (highest wins): SD_* env vars (debug), AC9_IMAGE_EDIT_ROLE
// bundle, hardcoded Chroma fallback. Read fresh every call so the
// Models settings tab's Save takes effect on the next edit without a
// restart.
Paths resolve_paths() {
    Paths p;
    p.sd_cli = env_or("SD_CLI_BIN",
        "/home/jwoods/work/Autoclank9001/scratchpad/sdcpp_build/sd/build/bin/sd-cli");

    std::string diffusion_default =
        "/home/jwoods/work/Autoclank9001/data/staging/Chroma1-HD-Q8_0.gguf";
    std::string vae_default =
        "/home/jwoods/work/Autoclank9001/data/staging/ae.safetensors";
    std::string encoder_default =
        "/home/jwoods/work/Autoclank9001/data/staging/t5-v1_1-xxl-encoder-Q8_0.gguf";
    std::string mmproj_default;
    std::string kind_default = "chroma";
    std::string model_args_default = "chroma_use_dit_mask=false";

    if (const char * role = std::getenv("AC9_IMAGE_EDIT_ROLE");
        role && *role) {
        if (auto bundle = data::role_image_bundle_paths(role)) {
            if (!bundle->kind.empty())         kind_default        = bundle->kind;
            if (!bundle->diffusion.empty())    diffusion_default   = bundle->diffusion.string();
            if (!bundle->vae.empty())          vae_default         = bundle->vae.string();
            if (!bundle->text_encoder.empty()) encoder_default     = bundle->text_encoder.string();
            if (!bundle->text_encoder_mmproj.empty())
                                                mmproj_default      = bundle->text_encoder_mmproj.string();
            model_args_default = bundle->model_args;
        }
    }
    p.kind                = kind_default;
    p.diffusion           = env_or("SD_CHROMA_MODEL", diffusion_default);
    p.vae                 = env_or("SD_FLUX_VAE",     vae_default);
    p.text_encoder        = env_or("SD_T5XXL",        encoder_default);
    p.text_encoder_mmproj = mmproj_default;
    p.model_args          = model_args_default;

    const bool is_qwen = p.kind == "qwen_image_edit";
    struct Check { const char * label; fs::path * path; bool required; };
    std::vector<Check> checks = {
        {"sd-cli",                                     &p.sd_cli,             true},
        {is_qwen ? "Qwen-Image-Edit DiT" : "Chroma DiT", &p.diffusion,        true},
        {is_qwen ? "Qwen VAE" : "Flux VAE",             &p.vae,               true},
        {is_qwen ? "Qwen2.5-VL text encoder"
                 : "T5-XXL text encoder",               &p.text_encoder,      true},
        {"Qwen2.5-VL mmproj",                           &p.text_encoder_mmproj, is_qwen},
    };
    for (const auto & c : checks) {
        if (!c.required) continue;
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
    return out.empty() ? std::string("edit") : out;
}

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
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        std::vector<char *> raw;
        raw.reserve(argv.size() + 1);
        for (const auto & a : argv) raw.push_back(const_cast<char *>(a.c_str()));
        raw.push_back(nullptr);
        execv(argv[0].c_str(), raw.data());
        std::fprintf(stderr, "execv failed: %s\n", std::strerror(errno));
        _exit(127);
    }
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
                   "ready to edit via img2img.";
    } else {
        s.ready  = false;
        s.detail = "image editor not ready - missing: " + p.missing;
    }
    return s;
}

Result edit(const std::string & input_image_path,
            const std::string & edit_prompt,
            const std::string & out_dir)
{
    Result r;
    const Paths p = resolve_paths();
    if (!p.missing.empty()) {
        r.ok = false;
        r.message = "image editor not ready: " + p.missing;
        return r;
    }
    std::error_code ec;
    if (input_image_path.empty() || !fs::exists(input_image_path, ec)) {
        r.ok = false;
        r.message = "no input image to edit "
                    "(would need a prior generated image in this session)";
        return r;
    }
    fs::create_directories(out_dir, ec);
    if (ec) {
        r.ok = false;
        r.message = "could not create output dir " + out_dir + ": " + ec.message();
        return r;
    }

    const std::string fname =
        slugify(edit_prompt) + "-edit-" + time_stamp() + ".png";
    const fs::path    out   = fs::path(out_dir) / fname;

    // Common argv (paths + prompt + output). Family-specific flags are
    // appended below by kind.
    std::vector<std::string> argv = {
        p.sd_cli.string(),
        "--diffusion-model", p.diffusion.string(),
        "--vae",             p.vae.string(),
        "-p",                edit_prompt,
    };

    if (p.kind == "qwen_image_edit") {
        // Qwen-Image-Edit-2511 native edit path. The reference image is
        // consumed as multimodal vision tokens through the Qwen2.5-VL
        // encoder pair, NOT as an img2img latent, so there is no
        // --strength blend - identity preservation is the point.
        // --model-args qwen_image_zero_cond_t=true is MANDATORY: without
        // it the VAE returns solid white (documented in leejet
        // docs/qwen_image_edit.md).
        argv.push_back("--llm");             argv.push_back(p.text_encoder.string());
        argv.push_back("--llm_vision");      argv.push_back(p.text_encoder_mmproj.string());
        argv.push_back("--cfg-scale");       argv.push_back("2.5");
        argv.push_back("--sampling-method"); argv.push_back("euler");
        argv.push_back("--steps");           argv.push_back("20");
        argv.push_back("--flow-shift");      argv.push_back("3");
        argv.push_back("--offload-to-cpu");
        argv.push_back("--diffusion-fa");
        argv.push_back("-r");                argv.push_back(input_image_path);
    } else {
        // Chroma img2img at strength 0.95 - the legacy fallback. Notes
        // preserved from the pre-refactor path: strength 0.75 leaves the
        // original kitten's colour white, so 0.95 is required for
        // "make it black" style prompts to actually re-paint.
        argv.push_back("--t5xxl");           argv.push_back(p.text_encoder.string());
        argv.push_back("--cfg-scale");       argv.push_back("4.0");
        argv.push_back("--sampling-method"); argv.push_back("euler");
        argv.push_back("--steps");           argv.push_back("20");
        argv.push_back("-H");                argv.push_back("1024");
        argv.push_back("-W");                argv.push_back("1024");
        argv.push_back("--strength");        argv.push_back("0.95");
        argv.push_back("--clip-on-cpu");
        argv.push_back("--vae-tiling");
        argv.push_back("-i");                argv.push_back(input_image_path);
    }
    if (!p.model_args.empty()) {
        argv.push_back("--model-args");
        argv.push_back(p.model_args);
    }
    argv.push_back("-o");
    argv.push_back(out.string());

    std::string log;
    std::lock_guard<std::mutex> lk(g_edit_mtx);
    // Evict every resident LLM so Chroma can allocate VRAM.
    evict_all_llms();
    // Same reason as image_generator: sd-cli is out of process and
    // won't tick our pulse counter, so the chat SSE heartbeat freezes
    // and the rainbow "thinking" animation stops. Use the NO-ROLE
    // PulseScope so we get the pulses without also forcing the
    // client into a "loading (Chroma1-HD)" label for the entire run -
    // the "image_edit" layer event already set the headline via
    // layerToRole and that stays honest through sampling.
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
    r.message    = "edited " + out.string();
    return r;
}

}
