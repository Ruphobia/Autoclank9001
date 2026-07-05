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

// CLI: `ac9 hw`   -> print enumerated GPUs, one line each.
// CLI: `ac9 plan` -> print current plan (or --refresh to re-plan first).
int cli_hw  (int argc, char ** argv);
int cli_plan(int argc, char ** argv);

}  // namespace hardware
