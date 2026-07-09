// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// data:: -- content-addressed asset store for ac9.
//
// Every large runtime asset (LLM weights, dictionaries, wikipedia ZIM,
// thesaurus, hazard/legal JSON) is stored as one or more <sha256>.bin
// chunks under `data/`, indexed by role in `data/manifest.json`.
//
// The chunker computes streaming SHA-256 of the input, splits it into
// <=1.5 GB pieces, renames each piece to <chunk-sha>.bin, and updates
// the manifest atomically.
//
// Rationale: GitHub Releases caps a single asset at 2 GiB, so chunk
// size is bounded from above; content-addressing gives us dedup,
// integrity verification, and cache-friendly hosting (URL == filename
// == sha).

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace data {

// Hex-encoded SHA-256 of an entire file (streamed, no whole-file
// buffer).
std::string sha256_file(const std::filesystem::path & p);

// Result of chunking one input asset. `chunk_shas` are in reassembly
// order.
struct ChunkResult {
    std::string              full_sha256;
    std::uintmax_t           size_bytes = 0;
    std::vector<std::string> chunk_shas;
};

// Chunk `input` into <chunk-sha>.bin files under `data_dir`, capped at
// `max_chunk_size` bytes per chunk. Idempotent: an existing chunk on
// disk with the matching SHA is left alone. `data_dir` is created if
// missing. Prints progress to stderr.
ChunkResult chunk_asset(
    const std::filesystem::path & input,
    const std::filesystem::path & data_dir,
    std::uintmax_t max_chunk_size = 1'500'000'000ULL);

// Atomically write/update the given role in <data_dir>/manifest.json.
// Uses flock to serialize concurrent writers.
void set_role_in_manifest(
    const std::filesystem::path & data_dir,
    const std::string           & role,
    const std::string           & human_name,
    const ChunkResult           & result);

// CLI entry point for `ac9 chunk <role> <human_name> <input> [data_dir]`.
// Returns process exit code.
int cli_chunk(int argc, char ** argv);

// Reassemble `role`'s chunks into a single file on disk and return its
// path. Cached: subsequent calls with the same role skip the reassembly
// and just return the cached path.
//
// The full-file sha256 is verified against the manifest before the
// cached path is returned; on mismatch the cache is deleted and rebuilt.
//
// Cache location: <data_dir>/cache/<sha>.gguf. Reassembly is atomic
// (via .tmp + rename). Concurrent calls for the same role serialize
// on a per-role mutex.
//
// Throws std::runtime_error if the role isn't in the manifest, or if a
// chunk is missing/corrupt.
std::filesystem::path resolve_role(
    const std::string & role,
    const std::filesystem::path & data_dir = "data");

// Unified role -> file resolver used by every LLM-loading subsystem.
// Search order:
//   1. `data_dir`/manifest.json chunk assembly (via resolve_role) - the
//      sha-addressed store. Rebuilds from <sha>.bin pieces if needed.
//   2. `resource_root`/<role>/*.gguf top-level: pick the largest file
//      that does NOT contain "mmproj" (that partner is served by
//      role_mmproj_path). Legacy resources/models/<role>/ layout.
//   3. `resource_root`/<role>/*.gguf reassembled from local
//      <base>.gguf.part-*.bin pieces via model_chunks::ensure().
// Throws std::runtime_error if none of the above resolves. Callers
// swap models by pointing AC9_<STAGE>_ROLE at any role that
// role_available() returns true for.
std::filesystem::path role_path(
    const std::string           & role,
    const std::filesystem::path & data_dir      = "data",
    const std::filesystem::path & resource_root = "resources/models");

// Mmproj (multi-modal projector) partner path for a role, or empty if
// the role has no mmproj file. Looks in resource_root/<role>/ for a
// *.gguf whose name contains "mmproj" (case-insensitive) or, failing
// that, tries the reassembly-from-parts path for the candidate name.
// Vision uses this to swap its projector alongside the LLM.
std::filesystem::path role_mmproj_path(
    const std::string           & role,
    const std::filesystem::path & data_dir      = "data",
    const std::filesystem::path & resource_root = "resources/models");

// True iff role_path(role) would return successfully without throwing.
// Cheap: does not reassemble anything; just checks that the ingredients
// are on disk.
bool role_available(
    const std::string           & role,
    const std::filesystem::path & data_dir      = "data",
    const std::filesystem::path & resource_root = "resources/models");

// One catalog entry from list_available_roles().
struct RoleInfo {
    std::string    role;          // manifest / directory key
    std::string    human_name;    // e.g. "ChemLLM-20B-Chat-DPO.i1-Q4_K_M.gguf"
    std::string    short_name;    // UI label; empty if unknown
    std::uintmax_t size_bytes = 0;
    std::string    source;        // "manifest" or "resource"
    bool           has_mmproj = false;
};

// Enumerate every role that is on-disk-usable right now (union of the
// manifest and the resources/models/<role>/ dirs). The models settings
// UI populates its per-flow dropdowns from this list.
std::vector<RoleInfo> list_available_roles(
    const std::filesystem::path & data_dir      = "data",
    const std::filesystem::path & resource_root = "resources/models");

}  // namespace data
