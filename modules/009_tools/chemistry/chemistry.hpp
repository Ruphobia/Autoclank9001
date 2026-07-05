#pragma once

#include <string>
#include <string_view>

// ChemLLM-20B-Chat-DPO Q4_K_M, pinned to GPU 1.
// Shares GPU 1 with coder + physics (three-way swap via cross-shutdown).
// REQUIRES prompt_cleanup::init() to have run first.
namespace chemistry {

void init();
void shutdown();

// Answer a chemistry question. ChemLLM is DPO-tuned for chemistry tasks
// (name conversion, molecular captioning, reaction prediction, IUPAC,
// SMILES, etc).
std::string answer(std::string_view question);

}

// Cross-shutdown handshakes (file-scope; pair with coder + physics).
extern "C" void chemistry_shutdown_if_loaded();
extern "C" void coder_shutdown_if_loaded();
extern "C" void physics_shutdown_if_loaded();
extern "C" void vision_shutdown_if_loaded();
