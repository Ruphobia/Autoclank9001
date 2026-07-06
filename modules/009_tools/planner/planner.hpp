// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// planner:: -- reasoning model that produces a plan before the coder writes
// code. Runs Qwen3-*-Thinking-abliterated by default; role selected via
// AC9_PLANNER_ROLE env var (defaults to "planner-4b"; also accepts
// "planner-30b" once that role is present in data/manifest.json).
//
// Model is resolved via data::resolve_role(), so chunks under data/ are
// reassembled + sha-verified on first load, cached under data/cache/.
//
// Output is stripped of the surrounding <think>...</think> block before
// return so callers see only the final plan.

#include <cstddef>
#include <string>
#include <string_view>

namespace planner {

// Blocking model load; safe to call multiple times.
void init();

// Free the runtime, freeing VRAM. Called from the cross-shutdown
// handshake when another GPU-1 tenant (coder/physics/chemistry/vision)
// needs to load.
void shutdown();

// Generate up to max_new_tokens against (system, user). Returns the
// model's final answer with the <think>...</think> block removed.
// Sets *truncated=true if the run stopped without hitting end-of-generation.
std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int              max_new_tokens,
                     bool           * truncated = nullptr);

}  // namespace planner

// Cross-shutdown export so coder/physics/chemistry/vision can evict
// planner from the shared GPU before they load.
extern "C" void planner_shutdown_if_loaded();
