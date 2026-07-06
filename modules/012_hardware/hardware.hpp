// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// hardware:: -- one-shot startup detection + per-role offload plan.
//
// enumerate_gpus() lists every CUDA device (via cudart if built with it,
// falling back to nvidia-smi popen).
//
// plan_roles(gpus, manifest_path) reads data/manifest.json and produces a
// JSON plan mapping each role to {gpu, n_gpu_layers, mode, ...}. Roles
// have implicit classes (HOT / SWAP / COLOCATE) based on a curated table
// in hardware.cpp; we do not yet mmap the GGUF header, we estimate VRAM
// from size_bytes + heuristic KV cache. Good enough for a first plan;
// GGUF-mmap upgrade is a follow-on once reassembly-on-demand lands.
//
// init() enumerates + plans + saves to ~/.ac9/gpu_plan.json. Every model
// loader can consult role_assignment("<role>") to get its plan entry.

#include <cstdint>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace hardware {

struct Gpu {
    int          id{-1};
    std::string  name;
    std::size_t  total_vram{0};
    std::size_t  free_vram{0};
    int          cc_major{0};
    int          cc_minor{0};
};

// Live per-GPU stats (temperature + utilization + memory). Fields are -1
// when unknown (e.g. no nvidia-smi available).
struct GpuStats {
    int          id{-1};
    std::string  name;
    int          temp_c{-1};
    int          util_pct{-1};
    std::uint64_t mem_used{0};
    std::uint64_t mem_total{0};
    std::string  role;              // which pick_placement role is resident (empty = idle)
};

std::vector<GpuStats> query_gpu_stats();

// Live system stats (RAM + CPU + package temperature). All best-effort;
// -1 fields mean "unknown."
struct SystemStats {
    std::uint64_t mem_used{0};
    std::uint64_t mem_total{0};
    int           cpu_pct{-1};        // averaged CPU utilization %
    int           temp_c{-1};         // hottest zone
    int           n_cpus{0};
};

SystemStats query_system_stats();

// Detected GPUs (CUDA runtime first, nvidia-smi fallback). Empty vector
// means "CPU only" -- the planner degrades gracefully.
std::vector<Gpu> enumerate_gpus();

// Build a per-role assignment plan from the manifest's size_bytes column.
// manifest_path is normally data/manifest.json.
nlohmann::json plan_roles(const std::vector<Gpu> & gpus,
                          const std::filesystem::path & manifest_path);

// Persist / read back the plan JSON.
void            save_plan(const nlohmann::json &, const std::filesystem::path &);
nlohmann::json  load_plan(const std::filesystem::path &);

// Startup entry: enumerate + plan + save to ~/.ac9/gpu_plan.json. Prints a
// short summary to stderr. Non-throwing.
void init();

// Consulted by model loaders. Returns the plan entry for `role` or a
// safe default ({gpu:0, n_gpu_layers:-1 (all), mode:"FULL", mmap:true})
// if no plan is available.
nlohmann::json role_assignment(const std::string & role);

// Single-card LRU scheduler. Each model loads onto exactly ONE GPU;
// consecutive model switches rotate to a different card so the previous
// role can stay resident (mmap warm) on its card for a potential re-use.
// If the model would exceed the chosen card's VRAM budget, n_gpu_layers
// is clipped so the overflow streams from the mmapped GGUF (still fast
// per jwoods's ollama bench, and always faster than LAYER-splitting
// across the PCIe boundary).
struct Placement {
    int         main_gpu{0};
    int         n_gpu_layers{-1};   // -1 = all layers on GPU (UVA handles overflow)
    bool        split_across{false};// true only if model_bytes exceeds every card
    bool        mmap{true};
    std::string reason;             // human-readable "fits fully on gpu 1", etc.
    std::string displaced_role;     // if picked card is busy, that role's name
};

// Ask the scheduler which card to load `role` on. Records the intent so
// the NEXT role_placement() call rotates to a different card. Pass the
// approximate on-disk size of the gguf and expected KV-cache headroom;
// the scheduler subtracts KV+activation reserve from the card's VRAM
// budget before deciding how many layers to offload.
Placement pick_placement(const std::string & role,
                         std::uint64_t model_bytes,
                         std::uint64_t kv_reserve_bytes = 2ULL << 30);

// Called by a model loader after llama_model_load_from_file returns
// successfully, so subsequent picks account for VRAM already spent.
void note_role_loaded(const std::string & role, int gpu);

// Called by a shutdown_if_loaded eviction; frees the card back to the LRU.
void note_role_unloaded(const std::string & role);

// Fire the shutdown_if_loaded hook for whatever role is named. Used by
// loaders that just got a Placement with a non-empty displaced_role:
// they call request_evict(placement.displaced_role) instead of blanket-
// evicting every sibling. No-op for empty / unknown role.
void request_evict(const std::string & role);

// CLI: `ac9 hw`   -> print enumerated GPUs, one line each.
// CLI: `ac9 plan` -> print current plan (or --refresh to re-plan first).
int cli_hw  (int argc, char ** argv);
int cli_plan(int argc, char ** argv);

}  // namespace hardware
