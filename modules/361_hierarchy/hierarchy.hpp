// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <vector>

// Multi-sheet hierarchical schematic support.
// The Schematic model already carries a `child_screens` map keyed by
// SchSheet UUID. This module handles:
//   * loading sub-sheets recursively when a SchSheet points at
//     Sheetfile fields
//   * flattening a hierarchy into a single logical netlist source
//   * tracking a sheet-path stack for navigation (enter / leave)
namespace hierarchy {

struct LoadOptions {
    // How deep to descend into nested sheets (protects against cycles).
    int max_depth = 8;
    // Base directory for resolving relative Sheetfile paths.
    std::string base_dir;
};

struct LoadReport {
    std::size_t sheets_loaded = 0;
    std::vector<std::string> warnings;
};

// Walk every SchSheet in `sch`; for each one, resolve its Sheetfile
// field to a path under `opts.base_dir` and load that .kicad_sch into
// sch.child_screens[sheet_uuid].root. Recurses into loaded sheets.
LoadReport load_children(kicad_model::Schematic & sch, const LoadOptions & opts = {});

// Flatten every child screen's items into a single vector for
// downstream tools (netlist, ERC) that don't yet understand
// hierarchical paths. Uses UUID-prefixed refdes so sheets don't
// collide.
std::vector<kicad_model::ItemPtr> flatten(const kicad_model::Schematic & sch);

// Sheet navigation stack. Kept small and value-typed; the editor UI
// tracks the current path as a list of sheet UUIDs from root.
struct Path {
    // Root is the empty path; each entry is a SchSheet uuid.
    std::vector<kicad_model::UUID> stack;

    std::string display() const;
    bool at_root() const { return stack.empty(); }
    void enter(kicad_model::UUID sheet_uuid) { stack.push_back(std::move(sheet_uuid)); }
    void leave() { if (!stack.empty()) stack.pop_back(); }
};

// Given a schematic + path, return the target screen. Nullptr if the
// path names a non-existent sheet.
const kicad_model::SchScreen * screen_at(const kicad_model::Schematic & sch,
                                          const Path & path);

}
