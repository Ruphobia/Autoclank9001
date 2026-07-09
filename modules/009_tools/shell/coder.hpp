// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>

// Qwen2.5-Coder-14B-Instruct-abliterated, Q5_K_M, pinned to GPU 1.
// Loaded lazily on first generate() call. Distinct from 003_stylize/qwen14b
// (different model, different physical card) so the two can co-exist.
//
// REQUIRES that prompt_cleanup::init() has been called first to initialize
// the ggml backend.
namespace coder {

void init();
void shutdown();

// `truncated`, when non-null, is set true if generation stopped without
// reaching an end-of-generation token (context full, decode error, or
// max_new_tokens hit): the returned text is then a PREFIX of the intended
// output and must not be trusted as a complete artifact.
std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int max_new_tokens = 512,
                     bool * truncated = nullptr);

// Post-code comment pass. Reuses the ALREADY-LOADED coder runtime (no
// eviction, no reload) to re-emit `source` with:
//   1. A Doxygen-style header comment above every function definition
//      (\brief, \param, \return, \note as appropriate).
//   2. Verbose inline // comments on non-obvious lines explaining what
//      each control-flow branch / algorithmic step is doing and why.
// The model is instructed to preserve every character of executable
// code verbatim; only comments and docstrings are inserted. If the
// model deviates, callers should fall back to the raw source.
std::string comment_code(std::string_view language_hint,
                         std::string_view source,
                         int max_new_tokens = 4096,
                         bool * truncated = nullptr);

// Which role name is active for the coder module. Read from the
// AC9_CODER_ROLE env var at first call ("coder" -> Qwen2.5-Coder-14B
// abliterated Q5_K_M, "coder-big" -> Qwen3-Coder-30B Q4_K_M / whatever
// is symlinked to resources/models/coder/coder-big.gguf). Public so
// the interface layer can label the widget with the right short name.
std::string active_role();

}
