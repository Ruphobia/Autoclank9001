// SPDX-License-Identifier: GPL-3.0-or-later
#include "gerber_and_fab_output_viewer.hpp"

#include "../340_kicad_bridge/kicad_bridge.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

namespace gerber_and_fab_output_viewer {
namespace {

std::mutex g_mtx;
bool       g_ready = false;

bool ensure_dir(const std::string & p) {
    if (p.empty()) return false;
    struct stat st{};
    if (::stat(p.c_str(), &st) == 0) return S_ISDIR(st.st_mode);
    return ::mkdir(p.c_str(), 0755) == 0;
}

} // namespace

std::vector<LayerSpec> default_layers_2layer() {
    return {
        {"F.Cu",     "Front Copper",   "#c88a17", true},
        {"B.Cu",     "Back Copper",    "#4d7fc4", true},
        {"F.Paste",  "Front Paste",    "#808080", false},
        {"B.Paste",  "Back Paste",     "#808080", false},
        {"F.SilkS",  "Front Silk",     "#dedede", true},
        {"B.SilkS",  "Back Silk",      "#dedede", true},
        {"F.Mask",   "Front Mask",     "#4bb051", false},
        {"B.Mask",   "Back Mask",      "#4bb051", false},
        {"Edge.Cuts","Board Outline",  "#f2f200", true},
        {"F.CrtYd",  "Front Courtyard","#7f7f7f", false},
        {"B.CrtYd",  "Back Courtyard", "#7f7f7f", false},
        {"F.Fab",    "Front Fab",      "#a08040", false},
        {"B.Fab",    "Back Fab",       "#a08040", false}
    };
}

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_ready) return;
    kicad_bridge::init();
    g_ready = true;
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_ready = false;
}

Status status() {
    Status s;
    s.ready  = g_ready;
    s.detail = g_ready ? "gerber viewer initialized" : "not initialized";
    return s;
}

RenderResult render(std::string_view pcb_path,
                    std::string_view out_dir,
                    const std::vector<LayerSpec> & layers) {
    RenderResult r;
    if (!kicad_bridge::config().available) {
        r.log = "kicad-cli not available; cannot render gerbers";
        return r;
    }
    std::string dir(out_dir);
    if (!ensure_dir(dir)) {
        r.log = "cannot create output dir: " + dir;
        return r;
    }

    std::ostringstream log;
    for (const auto & L : layers) {
        std::string safe = L.name;
        for (auto & c : safe) if (c == '.') c = '_';
        std::string svg = dir + "/" + safe + ".svg";
        auto rr = kicad_bridge::pcb_export_svg(pcb_path, svg, {L.name});
        log << "[" << L.name << "] exit=" << rr.exit_code
            << " out=" << rr.output_path;
        if (!rr.stderr_text.empty()) log << " err=" << rr.stderr_text;
        log << "\n";
        if (rr.exit_code == 0) r.svg_paths.push_back(svg);
    }

    // Combined multi-layer SVG (all requested layers overlaid).
    std::vector<std::string> names;
    for (const auto & L : layers) names.push_back(L.name);
    std::string combined = dir + "/combined.svg";
    auto rr = kicad_bridge::pcb_export_svg(pcb_path, combined, names);
    if (rr.exit_code == 0) r.combined_path = combined;

    r.log = log.str();
    r.ok  = !r.svg_paths.empty() || !r.combined_path.empty();
    return r;
}

std::string bundle_for_fab(std::string_view pcb_path, std::string_view work_dir) {
    if (!kicad_bridge::config().available) return {};
    std::string dir(work_dir);
    if (!ensure_dir(dir)) return {};

    std::string gerber_dir = dir + "/gerbers";
    if (!ensure_dir(gerber_dir)) return {};
    (void) kicad_bridge::pcb_export_gerbers(pcb_path, gerber_dir, {});
    (void) kicad_bridge::pcb_export_drill  (pcb_path, gerber_dir);
    (void) kicad_bridge::pcb_export_pos    (pcb_path, gerber_dir + "/pos.csv",
                                            "both", "csv");

    // Try to zip. If no `zip` in PATH, return the directory.
    std::string zip_path = dir + "/fab_bundle.zip";
    std::string cmd = "cd \"" + gerber_dir + "\" && zip -r \"" + zip_path + "\" . >/dev/null 2>&1";
    int rc = std::system(cmd.c_str());
    if (rc == 0) return zip_path;
    return gerber_dir;
}

} // namespace gerber_and_fab_output_viewer
