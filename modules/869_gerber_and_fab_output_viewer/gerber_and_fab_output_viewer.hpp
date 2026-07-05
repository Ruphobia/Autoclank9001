// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>
#include <vector>

// Preview + export fabrication output. The MVP delegates rendering to
// kicad-cli (see 340_kicad_bridge::pcb_export_svg) and manages the
// output cache, layer catalog, and per-layer color assignment consumed
// by the web viewer.
namespace gerber_and_fab_output_viewer {

// The standard fab-output layer set for a 2-layer board plus soldermask,
// silk, courtyard, and edge cuts. Rendered one file per layer.
struct LayerSpec {
    std::string name;           // KiCad canonical: "F.Cu", "F.SilkS", ...
    std::string display;        // human short name
    std::string css_color;      // "#c88a17" etc. Used by the web viewer.
    bool        default_visible = true;
};

std::vector<LayerSpec> default_layers_2layer();

void init();
void shutdown();

struct Status {
    bool        ready = false;
    std::string detail;
};
Status status();

// Given an existing .kicad_pcb, produce one SVG per layer under
// `out_dir` and return their filenames (in order matching `layers`).
// Requires 340_kicad_bridge to be available (kicad-cli).
struct RenderResult {
    std::vector<std::string> svg_paths;      // absolute paths, one per layer
    std::string              combined_path;  // multi-layer SVG when produced
    std::string              log;
    bool                     ok = false;
};

RenderResult render(std::string_view pcb_path,
                    std::string_view out_dir,
                    const std::vector<LayerSpec> & layers);

// Combine a fresh gerber export + drill export + pos file + job file
// into a zip suitable for uploading to a fab. Returns zip path.
// Requires an external zip binary; when unavailable returns the
// directory path instead.
std::string bundle_for_fab(std::string_view pcb_path,
                           std::string_view work_dir);

}
