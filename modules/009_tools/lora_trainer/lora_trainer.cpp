// SPDX-License-Identifier: GPL-3.0-or-later
#include "lora_trainer.hpp"

#include <array>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// Every resident LLM has to leave both GPUs before Chroma-scale training
// tries to allocate its 8.9B-param optimizer state. Same handles the
// image_generator module uses to boot the LLMs off shared cards.
extern "C" void coder_shutdown_if_loaded();
extern "C" void qwen14b_shutdown_if_loaded();
extern "C" void physics_shutdown_if_loaded();
extern "C" void chemistry_shutdown_if_loaded();
extern "C" void vision_shutdown_if_loaded();
extern "C" void planner_shutdown_if_loaded();

namespace lora_trainer {
namespace {

namespace fs = std::filesystem;

// One training run at a time. Two Chroma-LoRA trainers at once would
// OOM both P100s. Serialize here so a caller waiting on train() sees a
// clean queue.
std::mutex g_train_mtx;

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

std::string expand_home(std::string p) {
    if (p.size() >= 2 && p[0] == '~' && p[1] == '/') {
        if (const char * h = std::getenv("HOME")) {
            return std::string(h) + p.substr(1);
        }
    }
    return p;
}

// Canonical-slug rule: lowercase alphanumerics + underscore. Mirrors
// image_resolver::canonical_slug so filenames written here line up
// exactly with the reader helpers.
std::string canonical_slug(std::string_view raw) {
    std::string out;
    out.reserve(raw.size());
    bool last_us = false;
    for (char c : raw) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) {
            out.push_back(static_cast<char>(std::tolower(uc)));
            last_us = false;
        } else if (!last_us && !out.empty()) {
            out.push_back('_');
            last_us = true;
        }
    }
    while (!out.empty() && out.back() == '_') out.pop_back();
    return out;
}

fs::path canonical_root(std::string_view cwd, const std::string & slug) {
    const std::string proj = expand_home(std::string(cwd));
    if (proj.empty()) {
        if (const char * h = std::getenv("HOME")) {
            return fs::path(h) / ".ac9_images" / "canonical" / slug;
        }
        return fs::path("/tmp") / ".ac9_images" / "canonical" / slug;
    }
    return fs::path(proj) / ".ac9_images" / "canonical" / slug;
}

std::string default_python() {
    // Look for python3 on PATH. Just return the string; execvp will
    // resolve at exec time.
    return env_or("AC9_PYTHON_BIN", "python3");
}

std::string default_ai_toolkit_dir() {
    return env_or("AC9_AI_TOOLKIT_DIR",
        "/home/jwoods/work/Autoclank9001/scratchpad/ai-toolkit");
}

std::string default_base_model() {
    // Reuse Chroma1-HD from the image_generator module. If the operator
    // has a raw Chroma1-Base checkpoint elsewhere (ai-toolkit prefers
    // the safetensors distribution over gguf for training), let them
    // override via AC9_LORA_BASE_MODEL - otherwise fall back to the
    // gguf sd-cli uses.
    return env_or("AC9_LORA_BASE_MODEL",
        env_or("SD_CHROMA_MODEL",
            "/home/jwoods/work/Autoclank9001/data/staging/Chroma1-HD-Q8_0.gguf"));
}

// True when the interpreter runs successfully with --version. That is
// enough to know we can execv it; deeper checks (torch / accelerate)
// are the operator's job at install time.
bool python_available(const std::string & python_bin) {
    pid_t pid = fork();
    if (pid < 0) return false;
    if (pid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }
        execlp(python_bin.c_str(), python_bin.c_str(), "--version",
               (char *) nullptr);
        _exit(127);
    }
    int wstatus = 0;
    while (waitpid(pid, &wstatus, 0) < 0 && errno == EINTR) {}
    return WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0;
}

// Write a minimal ai-toolkit YAML training config. Follows the shape
// of ostris/ai-toolkit's config/examples/train_lora_flux_schnell_24gb.yaml
// but with Chroma1-HD as the model target, LoRA rank + steps from opts,
// and dataset_folder pointing at the canonical dir so every promoted
// image (plus its .txt caption if present) is picked up.
std::string build_train_yaml(const std::string & char_name,
                              const std::string & dataset_folder,
                              const std::string & output_folder,
                              const std::string & base_model,
                              const Options &     opts) {
    std::ostringstream y;
    y << "# ac9-generated ai-toolkit config for canonical `"
      << char_name << "`\n";
    y << "job: extension\n";
    y << "config:\n";
    y << "  name: " << char_name << "_lora\n";
    y << "  process:\n";
    y << "    - type: sd_trainer\n";
    y << "      training_folder: " << output_folder << "\n";
    y << "      device: cuda:0\n";
    y << "      trigger_word: " << char_name << "\n";
    y << "      network:\n";
    y << "        type: lora\n";
    y << "        linear: " << opts.rank << "\n";
    y << "        linear_alpha: " << opts.rank << "\n";
    y << "      save:\n";
    y << "        dtype: float16\n";
    y << "        save_every: " << opts.steps << "\n";
    y << "        max_step_saves_to_keep: 1\n";
    y << "      datasets:\n";
    y << "        - folder_path: " << dataset_folder << "\n";
    y << "          caption_ext: txt\n";
    y << "          caption_dropout_rate: 0.05\n";
    y << "          shuffle_tokens: false\n";
    y << "          cache_latents_to_disk: true\n";
    y << "          resolution:\n";
    y << "            - " << opts.resolution << "\n";
    y << "      train:\n";
    y << "        batch_size: 1\n";
    y << "        steps: " << opts.steps << "\n";
    y << "        gradient_accumulation_steps: 1\n";
    y << "        train_unet: true\n";
    y << "        train_text_encoder: false\n";
    y << "        gradient_checkpointing: true\n";
    y << "        noise_scheduler: flowmatch\n";
    y << "        optimizer: adamw8bit\n";
    y << "        lr: " << opts.learning_rate << "\n";
    y << "        dtype: bf16\n";
    y << "      model:\n";
    y << "        name_or_path: " << base_model << "\n";
    y << "        is_flux: true\n";
    y << "        quantize: true\n";
    y << "      sample:\n";
    y << "        sampler: flowmatch\n";
    y << "        sample_every: " << opts.steps << "\n";
    y << "        width: " << opts.resolution << "\n";
    y << "        height: " << opts.resolution << "\n";
    y << "        seed: 42\n";
    y << "        prompts:\n";
    y << "          - " << char_name << " standing pose\n";
    y << "meta:\n";
    y << "  name: " << char_name << "_lora\n";
    y << "  version: '1.0'\n";
    return y.str();
}

// Ensure the training folder contains a caption .txt for every image.
// ai-toolkit will use these as the captions; if a per-image caption is
// missing we drop the character-level tag line in as a fallback.
void ensure_captions(const std::string &              folder,
                     const std::string &              fallback_caption,
                     const std::vector<std::string> & image_paths) {
    for (const auto & p : image_paths) {
        fs::path png(p);
        fs::path caption = png;
        caption.replace_extension(".txt");
        std::error_code ec;
        if (fs::exists(caption, ec)) continue;
        std::ofstream out(caption);
        if (!out) continue;
        out << fallback_caption;
    }
    (void) folder;
}

// Fork + exec the trainer with argv, streaming its combined stdout+stderr
// via `on_line` (one line at a time) into both `log` (for the tail) and
// the caller's progress callback. Returns exit code, -1 on spawn failure.
int run_trainer(const std::vector<std::string> & argv,
                const std::vector<std::string> & envp_extra,
                std::string &                    log,
                const ProgressCb &               on_progress) {
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
        // Layer any extra env vars on top of the inherited environment.
        for (const auto & kv : envp_extra) {
            const std::size_t eq = kv.find('=');
            if (eq == std::string::npos) continue;
            setenv(kv.substr(0, eq).c_str(),
                   kv.substr(eq + 1).c_str(), 1);
        }
        std::vector<char *> raw;
        raw.reserve(argv.size() + 1);
        for (const auto & a : argv) raw.push_back(const_cast<char *>(a.c_str()));
        raw.push_back(nullptr);
        execvp(argv[0].c_str(), raw.data());
        std::fprintf(stderr, "execvp failed: %s\n", std::strerror(errno));
        _exit(127);
    }
    close(pipefd[1]);
    std::string line_buf;
    std::array<char, 4096> chunk;
    while (true) {
        ssize_t n = read(pipefd[0], chunk.data(), chunk.size());
        if (n > 0) {
            log.append(chunk.data(), static_cast<std::size_t>(n));
            for (ssize_t i = 0; i < n; ++i) {
                const char c = chunk[i];
                if (c == '\n') {
                    if (on_progress) on_progress(line_buf);
                    line_buf.clear();
                } else if (c != '\r') {
                    line_buf.push_back(c);
                }
            }
        } else if (n == 0) {
            break;
        } else if (errno == EINTR) {
            continue;
        } else {
            break;
        }
    }
    close(pipefd[0]);
    if (!line_buf.empty() && on_progress) on_progress(line_buf);
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

Status status() {
    Status s;
    const std::string py = default_python();
    const std::string tk = default_ai_toolkit_dir();
    std::vector<std::string> missing;
    if (!python_available(py)) {
        missing.push_back("python (`" + py + "` - try `sudo apt install python3`)");
    }
    std::error_code ec;
    if (!fs::is_directory(tk, ec)) {
        missing.push_back("ai-toolkit (`" + tk + "` - `git clone "
            "https://github.com/ostris/ai-toolkit` there, then `pip "
            "install -r requirements.txt` inside a venv)");
    }
    if (missing.empty()) {
        s.ready = true;
        s.detail = "python + ai-toolkit present; LoRA training ready.";
    } else {
        s.ready = false;
        s.detail = "LoRA trainer not ready - missing: ";
        for (std::size_t i = 0; i < missing.size(); ++i) {
            if (i) s.detail += "; ";
            s.detail += missing[i];
        }
    }
    return s;
}

Result train(std::string_view                  cwd,
             std::string_view                  char_name,
             const std::vector<std::string> &  image_paths,
             const Options &                   opts_in,
             ProgressCb                        on_progress) {
    Result r;
    Options opts = opts_in;
    if (opts.base_model_path.empty()) opts.base_model_path = default_base_model();
    if (opts.ai_toolkit_dir.empty())  opts.ai_toolkit_dir  = default_ai_toolkit_dir();
    if (opts.python_bin.empty())      opts.python_bin      = default_python();

    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) {
        r.ok = false;
        r.message = "invalid character name";
        return r;
    }
    if (image_paths.empty()) {
        r.ok = false;
        r.message = "no training images provided";
        return r;
    }
    if (!python_available(opts.python_bin)) {
        r.ok = false;
        r.message = "python missing - install ai-toolkit deps first "
                    "(python3 + `pip install -r "
                    "$AC9_AI_TOOLKIT_DIR/requirements.txt`)";
        return r;
    }
    std::error_code ec;
    if (!fs::is_directory(opts.ai_toolkit_dir, ec)) {
        r.ok = false;
        r.message = "ai-toolkit missing at `" + opts.ai_toolkit_dir +
                    "` - clone https://github.com/ostris/ai-toolkit "
                    "there and install its requirements.txt";
        return r;
    }
    // The ai-toolkit entry point.
    const fs::path run_py = fs::path(opts.ai_toolkit_dir) / "run.py";
    if (!fs::exists(run_py, ec)) {
        r.ok = false;
        r.message = "ai-toolkit entry point missing at `" + run_py.string() +
                    "` - is the checkout complete?";
        return r;
    }
    if (!fs::exists(opts.base_model_path, ec)) {
        r.ok = false;
        r.message = "base model missing at `" + opts.base_model_path +
                    "`; set AC9_LORA_BASE_MODEL or SD_CHROMA_MODEL";
        return r;
    }

    // Canonical directory doubles as the dataset folder: every promoted
    // .png sits there beside its optional .txt caption, exactly the
    // shape ai-toolkit expects. Output goes into a `training/` sibling
    // dir so the .lora.safetensors is easy to grep for.
    const fs::path root       = canonical_root(cwd, slug);
    const fs::path dataset    = root;
    const fs::path out_root   = root / "training";
    const fs::path config_yml = root / "train.yaml";
    const fs::path log_file   = root / "train.log";
    fs::create_directories(out_root, ec);
    if (ec) {
        r.ok = false;
        r.message = "could not create training output dir " +
                    out_root.string() + ": " + ec.message();
        return r;
    }

    // Every image gets a caption. Pull the character-level tag line
    // when available so unknown images still train against the right
    // language.
    std::string fallback_caption = slug + " character";
    const fs::path tag_file = root / (slug + ".txt");
    {
        std::ifstream in(tag_file);
        if (in) {
            std::stringstream ss;
            ss << in.rdbuf();
            std::string tl = ss.str();
            while (!tl.empty() &&
                   std::isspace(static_cast<unsigned char>(tl.back())))
                tl.pop_back();
            if (!tl.empty()) fallback_caption = slug + ", " + tl;
        }
    }
    ensure_captions(dataset.string(), fallback_caption, image_paths);

    // Write the YAML config. We rebuild it every run so option changes
    // (rank, steps, resolution) take effect immediately.
    {
        std::ofstream cfg(config_yml, std::ios::trunc);
        if (!cfg) {
            r.ok = false;
            r.message = "could not write train.yaml at " + config_yml.string();
            return r;
        }
        cfg << build_train_yaml(slug, dataset.string(), out_root.string(),
                                opts.base_model_path, opts);
    }

    // Build argv: `python3 <ai-toolkit>/run.py <train.yaml>`.
    std::vector<std::string> argv = {
        opts.python_bin,
        run_py.string(),
        config_yml.string(),
    };
    // Env: enable CUDA UVA so the optimizer state can overflow to RAM
    // on 16 GB P100s, per CLAUDE.md hardware discipline.
    std::vector<std::string> envp_extra = {
        "GGML_CUDA_ENABLE_UNIFIED_MEMORY=1",
        "CUDA_VISIBLE_DEVICES=0",
        // ai-toolkit's LoRA network module uses PyTorch dataloader
        // workers; single-worker is safe on Pascal.
        "TOKENIZERS_PARALLELISM=false",
    };

    // Serialize. Two trainers at once would OOM.
    std::lock_guard<std::mutex> lk(g_train_mtx);
    evict_all_llms();

    std::string log;
    if (on_progress) {
        on_progress("starting ai-toolkit trainer at " + run_py.string());
        on_progress("dataset: " + dataset.string() +
                    " (" + std::to_string(image_paths.size()) + " image(s))");
        on_progress("config: " + config_yml.string());
        on_progress("output: " + out_root.string());
    }
    int rc = run_trainer(argv, envp_extra, log, on_progress);

    // Mirror the log to disk for post-mortem.
    {
        std::ofstream lf(log_file, std::ios::trunc);
        if (lf) lf << log;
    }
    r.log_tail = tail(log);
    if (rc != 0) {
        r.ok = false;
        r.message = "ai-toolkit exited " + std::to_string(rc);
        return r;
    }

    // Search the output dir for the newest .safetensors and move it
    // into place as <slug>.lora.safetensors. ai-toolkit writes into
    // out_root/<config.name>/ typically but the exact layout has drifted
    // across versions - recursive scan is the safest way to land it.
    fs::path best;
    fs::file_time_type best_mtime{};
    for (auto it = fs::recursive_directory_iterator(out_root, ec);
         !ec && it != fs::recursive_directory_iterator(); ++it) {
        if (!it->is_regular_file()) continue;
        if (it->path().extension() != ".safetensors") continue;
        std::error_code mec;
        auto mt = fs::last_write_time(it->path(), mec);
        if (mec) continue;
        if (best.empty() || mt > best_mtime) {
            best = it->path();
            best_mtime = mt;
        }
    }
    if (best.empty()) {
        r.ok = false;
        r.message = "ai-toolkit reported success but no .safetensors "
                    "was found under " + out_root.string();
        return r;
    }
    const fs::path landed = root / (slug + ".lora.safetensors");
    fs::copy_file(best, landed, fs::copy_options::overwrite_existing, ec);
    if (ec) {
        r.ok = false;
        r.message = "could not copy trained LoRA into place: " + ec.message();
        return r;
    }
    r.ok        = true;
    r.lora_path = landed.string();
    r.message   = "trained LoRA saved to " + landed.string();
    return r;
}

}  // namespace lora_trainer
