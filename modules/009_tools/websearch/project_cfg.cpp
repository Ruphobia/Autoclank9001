// SPDX-License-Identifier: GPL-3.0-or-later
#include "project_cfg.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace project_cfg {
namespace {

// Canonical filename (what set_web_lookup writes). ac9-branded name.
constexpr const char * kFileName = ".ac9ai.cfg";
// Legacy filename kept read-compatible so users migrating from the older
// "tool" branding do not silently lose their per-project setting when
// the canonical file is absent.
constexpr const char * kLegacyFileName = ".toolai.cfg";

json load_one(const fs::path & p) {
    std::error_code ec;
    if (!fs::is_regular_file(p, ec)) return json::object();
    std::ifstream f(p, std::ios::binary);
    if (!f) return json::object();
    std::stringstream ss;
    ss << f.rdbuf();
    json j = json::parse(ss.str(), nullptr, /*allow_exceptions=*/false);
    if (!j.is_object()) return json::object();
    return j;
}

// Load the cfg as a JSON object. Prefer .ac9ai.cfg (canonical); if it's
// absent, read the legacy .toolai.cfg instead. Empty object on any
// failure so callers get a defined shape.
json load(std::string_view project_root) {
    if (project_root.empty()) return json::object();
    const fs::path root(std::string{project_root});
    json j = load_one(root / kFileName);
    if (!j.empty()) return j;
    return load_one(root / kLegacyFileName);
}

}  // namespace

std::string cfg_path(std::string_view project_root) {
    if (project_root.empty()) return {};
    return (fs::path(std::string(project_root)) / kFileName).string();
}

bool web_lookup_enabled(std::string_view project_root) {
    const json j = load(project_root);
    auto it = j.find("web_lookup");
    return it != j.end() && it->is_boolean() && it->get<bool>();
}

bool set_web_lookup(std::string_view project_root, bool enabled) {
    if (project_root.empty()) return false;
    json j = load(project_root);           // preserve any other keys
    j["web_lookup"] = enabled;

    const fs::path p = fs::path(std::string(project_root)) / kFileName;
    std::error_code ec;
    fs::create_directories(p.parent_path(), ec);

    // Atomic-ish write: tmp + rename so a crash can't leave a half file.
    const fs::path tmp = p.string() + ".tmp";
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f) return false;
        f << j.dump(2) << "\n";
        if (!f) return false;
    }
    fs::rename(tmp, p, ec);
    if (ec) {
        std::error_code rec;
        fs::remove(tmp, rec);
        return false;
    }
    return true;
}

}  // namespace project_cfg
