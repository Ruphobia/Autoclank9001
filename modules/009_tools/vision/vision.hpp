#pragma once

#include <string>
#include <string_view>

// Qwen3-VL-8B-Instruct-abliterated, Q5_K_M + f16 vision projector.
// Pinned to GPU 1. Shares GPU 1 with coder / physics / chemistry (four-way
// swap via cross-shutdown handshake). Loaded lazily on first describe().
namespace vision {

void init();
void shutdown();

// Run the vision-language model over `image_path` with `prompt`.
// Returns the model's textual description / answer.
// Throws on missing files or load failure.
std::string describe(std::string_view image_path, std::string_view prompt);

}

// Cross-shutdown handshakes — vision shares GPU 1 with coder/physics/chemistry.
extern "C" void vision_shutdown_if_loaded();
extern "C" void coder_shutdown_if_loaded();
extern "C" void physics_shutdown_if_loaded();
extern "C" void chemistry_shutdown_if_loaded();
