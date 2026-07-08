// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

// LoRA training loop against Chroma1-HD via ostris/ai-toolkit. This is
// the Week-2 half of the subject-consistency ship plan (Level 3 in
// scratchpad/subject_consistency_research.md §6). It shells out to a
// Python trainer the same way image_generator shells out to sd-cli:
// forked subprocess, argv-only, output piped back for the SSE stream.
//
// PREREQUISITES the operator must install once, outside ac9:
//   * Python 3.10+
//   * `pip install -r <ai-toolkit>/requirements.txt` (torch, accelerate,
//     safetensors, transformers, xformers, ...).
//   * ostris/ai-toolkit cloned under AC9_AI_TOOLKIT_DIR (default:
//     /home/jwoods/work/Autoclank9001/scratchpad/ai-toolkit).
//   * Chroma1-HD base model already on disk (reused from SD_CHROMA_MODEL).
//
// If ANY of those are missing, train() must fail cleanly with a
// human-readable message - it must never crash the ac9 server.
namespace lora_trainer {

// Per-training tunables. Defaults match §5 of the research report:
// rank 16, 1000 steps, single-card at a time. On 2xP100 that's roughly
// 90-180 min wall-clock.
struct Options {
    // LoRA rank. 16 is the paved Chroma path; higher captures more
    // detail at the cost of file size and training time.
    int rank = 16;

    // Training step count. 500-1000 is the sweet spot for a fixed 2D
    // sprite / character subject.
    int steps = 1000;

    // Learning rate. 1e-4 is the ostris/ai-toolkit default for Chroma.
    double learning_rate = 1e-4;

    // Training resolution. 512 is fast on P100 (no tensor cores);
    // bump to 1024 when the operator has time to burn.
    int resolution = 512;

    // Base model path. Empty -> read SD_CHROMA_MODEL from the env, same
    // as image_generator does.
    std::string base_model_path;

    // ai-toolkit directory. Empty -> read AC9_AI_TOOLKIT_DIR or fall
    // back to scratchpad/ai-toolkit.
    std::string ai_toolkit_dir;

    // Python interpreter to invoke. Empty -> `python3` on PATH.
    std::string python_bin;
};

struct Result {
    bool        ok = false;
    // Absolute path to the trained .lora.safetensors on success. Lands
    // under <cwd>/.ac9_images/canonical/<char>/<char>.lora.safetensors
    // where image_gen picks it up automatically.
    std::string lora_path;
    // Human-readable one-liner. On failure: which check failed.
    std::string message;
    // Last few KB of the trainer's combined stdout+stderr. Goes into
    // the SSE stream so the AI-pane can show the tail.
    std::string log_tail;
};

// Progress callback. Called with one line at a time from the trainer's
// stdout as it streams. Callers pass in an emit-to-SSE lambda so the
// AI pane can show step counts ("step 400/1000 loss=0.148").
using ProgressCb = std::function<void(const std::string & line)>;

// Kick off training. Blocks until the trainer subprocess exits.
// `char_name` is the character slug (matches image_resolver's slug).
// `image_paths` is the training set (absolute paths). `on_progress`
// may be nullptr. Never throws - errors go into the Result.
Result train(std::string_view                  cwd,
             std::string_view                  char_name,
             const std::vector<std::string> &  image_paths,
             const Options &                   opts,
             ProgressCb                        on_progress = nullptr);

// Cheap check: does the operator have Python + ai-toolkit installed?
// Used by status()-style callers so the AI pane can surface a "install
// ai-toolkit first" hint before the operator even tries to train.
struct Status {
    bool        ready = false;
    std::string detail;
};
Status status();

}  // namespace lora_trainer
