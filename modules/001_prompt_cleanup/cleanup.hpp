#pragma once

#include <string>
#include <string_view>

namespace prompt_cleanup {

// Loads the GGUF, creates the llama context, and pins to the chosen CUDA
// device. Idempotent and thread-safe. Reads env var TOOL_PROMPT_CLEANUP_GPU
// (default: "0") for the GPU index. Sets CUDA_VISIBLE_DEVICES internally
// before any CUDA probe; MUST run before any other CUDA-using code in the
// process.
void init();

// Free the model + context. Intentionally does NOT call llama_backend_free()
// (which would tear down CUDA state for the process).
void shutdown();

// Run a single cleanup turn. Returns the corrected text with leading and
// trailing whitespace stripped. Throws std::runtime_error on unrecoverable
// failures (model file missing, CUDA init failed, etc.).
//
// Concurrent callers serialize on an internal mutex.
std::string clean(std::string_view input);

}
