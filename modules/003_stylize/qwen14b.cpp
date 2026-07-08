// SPDX-License-Identifier: GPL-3.0-or-later
#include "qwen14b.hpp"

// Historical wrapper for Qwen2.5-14B. The 14B has been retired from the
// pipeline: every understanding stage (classify, entities, expertise,
// disambiguate, stylize, render_final, resolve, answer, planner, task
// planning, web-lookup intent, parts intent, ...) now delegates through
// this file to coder::generate, which is qwen35 (Qwen3.6-35B-A3B
// Claude-4.7-Opus abliterated, layer-split across both P100s). One
// resident model handles all thinking + all coding.
//
// The old file loaded a distinct 14B GGUF here. That has been removed
// so we don't hold ~15 GB of VRAM for a model that's no longer being
// dispatched to. The qwen14b_shutdown_if_loaded() hook that other
// modules call to evict "the 14B" is now a no-op - there is nothing
// to evict.
#include "../009_tools/shell/coder.hpp"

#include <string>
#include <string_view>

namespace qwen14b {

void init() {
    // No 14B to load. Coder (qwen35) loads lazily on first generate().
}

void shutdown() {
    // Nothing owned. coder::shutdown() handles the qwen35 runtime.
}

std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int max_new_tokens) {
    // Straight delegation. The coder module owns the layer-split qwen35
    // runtime; every historical qwen14b caller now runs on it.
    return coder::generate(system_prompt, user_msg, max_new_tokens,
                           /*truncated=*/nullptr);
}

}  // namespace qwen14b

// Cross-shutdown handshake export: still exists so callers who reference
// it link cleanly, but there's no state to release.
extern "C" void qwen14b_shutdown_if_loaded() {
    // no-op - the 14B has been retired
}
