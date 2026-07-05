#pragma once

#include <string>
#include <string_view>

// Qwen3-14B-abliterated, Q5_K_M, pinned to GPU 1.
// Shares GPU 1 with the coder model (both ~10 GB, P100 only has 16 GB);
// loading physics evicts coder and vice versa via the cross-shutdown
// helpers declared in both files.
//
// REQUIRES prompt_cleanup::init() to have been called first.
namespace physics {

void init();
void shutdown();

// Answer a physics question. Uses Qwen3's thinking mode for derivations;
// the leading <think>...</think> reasoning block is stripped before
// returning. Concurrent callers serialize.
std::string answer(std::string_view question);

}

// Cross-shutdown handshake: physics needs to evict coder before loading,
// and vice versa. Declared at file scope (no namespace) so they're easy
// to forward-declare from the other side.
extern "C" void physics_shutdown_if_loaded();
extern "C" void coder_shutdown_if_loaded();
extern "C" void chemistry_shutdown_if_loaded();
extern "C" void vision_shutdown_if_loaded();
