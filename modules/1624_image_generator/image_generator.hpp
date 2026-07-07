// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>

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
};

void init();
void shutdown();

// Reports whether all the pieces (sd-cli binary + Chroma model + VAE + T5 encoder)
// are on disk and ready. The AI-pane picks up on ready=true / false.
Status status();

// Generate an image for the given prompt into out_dir (any writable directory).
// Blocks until sd-cli finishes. Never throws — errors go into the Result.
Result generate(const std::string & prompt,
                const std::string & out_dir);

}
