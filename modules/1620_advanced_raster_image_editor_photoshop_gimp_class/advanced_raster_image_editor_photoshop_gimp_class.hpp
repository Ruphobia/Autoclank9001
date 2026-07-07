// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>

// Real Qwen-Image-Edit backed raster editor. Shells out to sd-cli
// (built from stable-diffusion.cpp) which runs the Qwen-Image-Edit
// DiT + Qwen VAE + Qwen2.5-VL text/vision encoder to produce an
// edited PNG at `out_dir/<slug>-edit.png`.
namespace advanced_raster_image_editor_photoshop_gimp_class {

struct Status {
    bool        ready = false;
    std::string detail;
};

struct Result {
    bool        ok = false;
    std::string image_path;
    std::string message;
    std::string log_tail;
};

void init();
void shutdown();
Status status();

// Apply an editing instruction to `input_image_path`, writing the
// result under `out_dir`. Blocks; never throws.
Result edit(const std::string & input_image_path,
            const std::string & edit_prompt,
            const std::string & out_dir);

}
