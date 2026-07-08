// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Real Chroma1-HD backed image generator. Shells out to sd-cli (built
// from stable-diffusion.cpp) which runs the Chroma DiT + Flux VAE + T5
// XXL text encoder to produce a PNG at `out_dir/<slug>.png`.
namespace image_generator {

struct Status {
    bool        ready = false;
    std::string detail;
};

struct Result {
    bool        ok = false;
    std::string image_path;   // absolute path to the PNG on success
    std::string message;      // human-readable one-liner (routed / done / error reason)
    std::string log_tail;     // last few KB of sd-cli stderr; goes into the AI-pane layer
    // Populated when a seed was used (either supplied or auto-selected).
    // 0 means "unknown / not tracked" - old callers that use the legacy
    // signature will always see 0 here.
    std::uint64_t seed = 0;
};

// LoRA reference the operator wants applied to this generation. Rendered
// as `<lora:name:weight>` tokens appended to the prompt so sd-cli's
// existing `--lora-model-dir` + prompt-token path picks them up. The
// research report calls out ostris/ai-toolkit LoRAs as the paved Chroma
// consistency lever (see scratchpad/subject_consistency_research.md §1.4).
struct LoraRef {
    std::string name;      // basename matching a .safetensors under the lora dir
    double      weight = 1.0;
};

// Optional per-call knobs. Every field is optional; leaving it at its
// default preserves the legacy text-to-image behavior byte-for-byte.
// See scratchpad/subject_consistency_research.md §6 (Level 1 recipes).
struct Options {
    // Reproducible RNG seed. 0 means "let sd-cli pick" (legacy behavior).
    // Non-zero is passed as `-s <seed>`.
    std::uint64_t seed = 0;

    // Absolute path to an init image for img2img. Empty means pure
    // text-to-image (legacy behavior). Passed as `-i <path>`.
    std::string init_img_path;

    // img2img denoise strength (0.0 = keep init, 1.0 = fully re-diffuse).
    // The research report recommends 0.55 for identity-preserving
    // "same character, new pose" variants. Only sent when
    // init_img_path is non-empty; sd-cli then reads `--strength <f>`.
    double strength = 0.55;

    // LoRAs to append to the prompt as `<lora:name:weight>` tokens.
    // Populate the ostris/ai-toolkit output filename here for Week 2
    // canonical characters.
    std::vector<LoraRef> lora_refs;
};

void init();
void shutdown();

// Reports whether all the pieces (sd-cli binary + Chroma model + VAE + T5 encoder)
// are on disk and ready. The AI-pane picks up on ready=true / false.
Status status();

// Legacy signature. Preserved as a thin forwarder to the extended
// generate() below so every existing caller keeps compiling / linking
// without change. Uses default Options (no seed lock, no init image,
// no LoRAs) which reproduces the previous behavior byte-for-byte.
Result generate(const std::string & prompt,
                const std::string & out_dir);

// Extended signature. Same core sd-cli invocation as the legacy path,
// plus optional seed / img2img / LoRA knobs from the subject-consistency
// ship plan. Blocks until sd-cli finishes. Never throws - errors go into
// the Result. `opts.init_img_path` (if set) MUST exist on disk; missing
// files surface as a clean failure Result rather than a crash.
Result generate(const std::string & prompt,
                const std::string & out_dir,
                const Options &     opts);

}
