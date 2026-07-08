// SPDX-License-Identifier: GPL-3.0-or-later
#include "advanced_raster_image_editor_photoshop_gimp_class.hpp"

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

// The editor now shares the Chroma1-HD + ae.safetensors + T5-XXL stack
// with the generator. The prior Qwen-Image-Edit-2511 path (documented
// as the "correct" editor upstream) produced a 100% white output on
// every configuration we tried - condition graph completes, sampling
// completes, decode completes, but the VAE returns a solid RGB
// (255,255,255) image. Root cause is a compatibility gap between our
// mmproj-f16 file and the sd-cli QwenImageEditPlusPipeline for the
// 2511 diffusion. Chroma + img2img at strength 0.95 produces real
// edits (verified: a white kitten -> a black kitten in ~3 min at
// 512x512), so we use that.
struct Paths {
    fs::path sd_cli;
    fs::path chroma;
    fs::path vae;
    fs::path t5xxl;
    std::string missing;
};

Paths resolve_paths() {
    Paths p;
    p.sd_cli = env_or("SD_CLI_BIN",
        "/home/jwoods/work/Autoclank9001/scratchpad/sdcpp_build/sd/build/bin/sd-cli");
    p.chroma = env_or("SD_CHROMA_MODEL",
        "/home/jwoods/work/Autoclank9001/data/staging/Chroma1-HD-Q8_0.gguf");
    p.vae    = env_or("SD_FLUX_VAE",
        "/home/jwoods/work/Autoclank9001/data/staging/ae.safetensors");
    p.t5xxl  = env_or("SD_T5XXL",
        "/home/jwoods/work/Autoclank9001/data/staging/t5-v1_1-xxl-encoder-Q8_0.gguf");
    for (const auto & pr : {std::pair{"sd-cli",      &p.sd_cli},
                            std::pair{"Chroma model",&p.chroma},
                            std::pair{"Flux VAE",    &p.vae},
                            std::pair{"T5-XXL",      &p.t5xxl}}) {
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

    // Chroma img2img config (verified working):
    // - --strength 0.95 lets the noise re-generate almost freely so a
    //   "make it black" style prompt actually changes color; at 0.75
    //   the pipeline leaves the original kitten white.
    // - --backend keeps the T5 encoder + VAE on CPU (frees ~5 GB VRAM)
    //   so the ~10 GB Chroma DiT + its ~2 GB compute buffer sit on
    //   whichever card auto-fit picks. --split-mode layer would move
    //   Chroma across both cards but at 1024x1024 it's not required.
    // - 1024x1024 mirrors the generator dimensions so the edit and
    //   original are directly comparable in the AI pane.
    std::vector<std::string> argv = {
        p.sd_cli.string(),
        "--diffusion-model", p.chroma.string(),
        "--vae",             p.vae.string(),
        "--t5xxl",           p.t5xxl.string(),
        "--cfg-scale",       "4.0",
        "--sampling-method", "euler",
        "--steps",           "20",
        "-H",                "1024",
        "-W",                "1024",
        "--strength",        "0.95",
        "--clip-on-cpu",
        "--vae-tiling",
        "--model-args",      "chroma_use_dit_mask=false",
        "-i",                input_image_path,
        "-p",                edit_prompt,
        "-o",                out.string(),
    };

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
