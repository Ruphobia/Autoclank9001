#pragma once

#include <string>
#include <string_view>

// Shared 14B model wrapper. Owns the llama_model + llama_context for
// Qwen2.5-14B-Instruct-abliterated.Q5_K_M.gguf. Multiple "persona" modules
// (stylize, expertise, ...) call generate() with their own system prompts.
//
// REQUIRES that prompt_cleanup::init() has been called first to initialize
// the ggml/llama backend and set CUDA_VISIBLE_DEVICES.
namespace qwen14b {

void init();
void shutdown();

// Single-turn generation: applies the model's chat template to
// (system, user), runs greedy decode to EOS or max_new_tokens.
// Returns the generated text, stripped of leading/trailing whitespace.
// Concurrent callers serialize on an internal mutex (shared context).
std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int max_new_tokens = 512);

}
