// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Server-side library authoring: create/edit LibSymbol and Footprint
// objects held in a per-session scratch library, then commit either
// into the session's Schematic.lib_symbols (for symbols) or as
// standalone Footprint items to be re-used across a project.
//
// Persistence to on-disk .kicad_sym / .kicad_mod files is a follow-up
// (needs library path resolution + write permission checks).
namespace lib_editor {

// --- Symbol editor scratch state ------------------------------------

// Editing a lib_symbol: we hold a full LibSymbol plus a "dirty" flag
// so the client can display an unsaved indicator.
struct SymbolDoc {
    kicad_model::LibSymbol lib;
    bool dirty = false;
};

// Add a pin to `doc.lib`. Returns the new SchPin's uuid.
std::string add_pin(SymbolDoc & doc, kicad_model::SchPin pin);
bool        remove_pin(SymbolDoc & doc, std::string_view uuid);
bool        move_pin(SymbolDoc & doc, std::string_view uuid, long long dx_nm, long long dy_nm);

// Set a top-level field (Reference/Value/Datasheet/Footprint/custom).
void        set_field(SymbolDoc & doc, std::string name, std::string value);

// --- Footprint editor scratch state ---------------------------------

struct FootprintDoc {
    kicad_model::Footprint fp;
    bool dirty = false;
};

std::string add_pad(FootprintDoc & doc, kicad_model::Pad pad);
bool        remove_pad(FootprintDoc & doc, std::string_view uuid);
bool        move_pad(FootprintDoc & doc, std::string_view uuid, long long dx_nm, long long dy_nm);

// Attach a 3D model reference (path + placement) to the footprint.
// Stored as a raw (model "path" (offset...) (scale...) (rotate...))
// s-expression so writers preserve it verbatim.
void attach_3d_model(FootprintDoc & doc,
                     std::string_view path,
                     double off_x_mm = 0, double off_y_mm = 0, double off_z_mm = 0,
                     double rot_x_deg = 0, double rot_y_deg = 0, double rot_z_deg = 0,
                     double scale_x = 1, double scale_y = 1, double scale_z = 1);

// --- 3D importers ----------------------------------------------------

// Minimal WRL (VRML 1.0/2.0 ASCII) parser: extracts vertex + face
// counts so the UI can preview the model without a full mesh loader.
// Full mesh loading is a follow-up.
struct MeshSummary {
    std::string format;    // "wrl" | "x3d" | "step" | "stl" | "unknown"
    std::size_t vertices = 0;
    std::size_t faces    = 0;
    std::string title;
    // AABB in the file's own units (mm assumed).
    double lo_x = 0, lo_y = 0, lo_z = 0;
    double hi_x = 0, hi_y = 0, hi_z = 0;
};
MeshSummary summarize_mesh(std::string_view text_or_bytes, std::string_view hint_ext);

}
