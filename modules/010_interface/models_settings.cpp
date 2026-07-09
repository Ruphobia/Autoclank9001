// SPDX-License-Identifier: GPL-3.0-or-later
#include "models_settings.hpp"

#include <nlohmann/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>

namespace models_settings {
namespace {

namespace fs = std::filesystem;
using json    = nlohmann::json;

// Constructed lazily so std::string globals don't run before main().
const std::vector<FlowDef> & flow_defs_impl() {
    static const std::vector<FlowDef> v = {
        { "planner",     "Planner",
          "Ticket planning + decomposition + failure analysis (thinking).",
          "AC9_PLANNER_ROLE",       "qwen35", false },
        { "coder",       "Coder",
          "Writes source files and shell commands. Also backs "
          "classify / entities / expertise / stylize / disambiguate / answer "
          "(understanding stack delegates through coder::generate).",
          "AC9_CODER_ROLE",         "qwen35", false },
        { "tool_router", "Tool router",
          "LLM-based intent classifier that maps free-form chat to the "
          "right ac9 tool. Empty = no LLM router (legacy regex fallback).",
          "AC9_TOOL_ROUTER_ROLE",   "",       false },
        { "cleanup",     "Prompt cleanup",
          "Fast 1.5B copy editor that fixes spelling / grammar before "
          "user text hits the pipeline.",
          "AC9_CLEANUP_ROLE",       "cleanup", false },
        { "physics",     "Physics tool",
          "Domain answerer for physics questions.",
          "AC9_PHYSICS_ROLE",       "physics", false },
        { "chemistry",   "Chemistry tool",
          "Domain answerer for chemistry questions.",
          "AC9_CHEMISTRY_ROLE",     "chemistry", false },
        { "vision",      "Vision (LLM)",
          "Vision-language model that describes images. Pair it with a "
          "mmproj role in the Vision (mmproj) row below.",
          "AC9_VISION_ROLE",        "vision", false },
        { "vision_mmproj", "Vision (mmproj)",
          "Multi-modal projector partner file for the vision LLM. Usually "
          "the same role as Vision (LLM), which points at a dir holding "
          "both files.",
          "AC9_VISION_MMPROJ_ROLE", "vision", false },
        { "image_gen",     "Image generator",
          "Diffusion bundle sd-cli loads for text-to-image "
          "(DiT + VAE + text encoder). Bundle files come from the picked "
          "role's manifest.json image_bundle field; SD_* env vars still "
          "win as per-file debug overrides.",
          "AC9_IMAGE_GEN_ROLE",     "chroma1-hd", false },
        { "image_edit",    "Image editor",
          "Diffusion bundle sd-cli loads for img2img / inpaint (currently "
          "Chroma at strength 0.95).",
          "AC9_IMAGE_EDIT_ROLE",    "chroma1-hd", false },
    };
    return v;
}

std::mutex g_settings_mu;

json read_settings_locked() {
    fs::path path = "settings/models.json";
    std::error_code ec;
    if (!fs::is_regular_file(path, ec)) return json::object();
    std::ifstream f(path);
    if (!f) return json::object();
    json j;
    try { f >> j; } catch (...) { return json::object(); }
    if (!j.is_object()) return json::object();
    return j;
}

}  // namespace (anon)

const std::vector<FlowDef> & flow_defs() { return flow_defs_impl(); }

std::string effective_role_for(std::string_view flow_key) {
    const std::string key(flow_key);
    const auto & defs = flow_defs_impl();
    const FlowDef * def = nullptr;
    for (const auto & d : defs) if (d.key == key) { def = &d; break; }
    if (!def) return {};

    std::lock_guard<std::mutex> lk(g_settings_mu);
    json j = read_settings_locked();
    if (j.contains("flows") && j["flows"].is_object() &&
        j["flows"].contains(key) && j["flows"][key].is_string()) {
        const std::string v = j["flows"][key].get<std::string>();
        // Empty string is legitimate for tool_router (=off), so preserve it.
        return v;
    }
    return def->default_role;
}

void apply_from_disk_via_setenv() {
    std::lock_guard<std::mutex> lk(g_settings_mu);
    json j = read_settings_locked();
    if (!j.contains("flows") || !j["flows"].is_object()) return;
    const auto & flows = j["flows"];
    for (const auto & def : flow_defs_impl()) {
        if (def.env_var.empty()) continue;
        // Respect an existing env var so command-line overrides win over
        // settings/models.json. This preserves the operator's AC9_*_ROLE
        // debug workflow.
        if (const char * existing = std::getenv(def.env_var.c_str());
            existing && *existing) continue;
        if (!flows.contains(def.key) || !flows[def.key].is_string()) continue;
        const std::string v = flows[def.key].get<std::string>();
        // Setting an empty string is fine (unsets the override for
        // tool_router-style opt-outs). Skip if same-string identity so we
        // don't churn.
        ::setenv(def.env_var.c_str(), v.c_str(), 1);
        std::fprintf(stderr,
            "models_settings: %s=%s (from settings/models.json)\n",
            def.env_var.c_str(), v.c_str());
    }
}

}  // namespace models_settings
