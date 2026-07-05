#pragma once

#include <filesystem>

namespace model_chunks {

// Ensure `final_path` exists. If not, look for sibling files matching
// `<final_path>.part-*.bin` (lexicographically sorted), concatenate them
// into `final_path`, and return.
//
// Returns true if `final_path` is ready for loading on return. Returns
// false (and prints to stderr) if neither the file nor any chunks are
// found.  Throws std::runtime_error on I/O failure mid-reassembly.
//
// Idempotent: once the final file exists, subsequent calls are a no-op.
bool ensure(const std::filesystem::path & final_path);

}
