// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Reference-designator annotator: assign U1/R1/C1/... to every
// SchSymbol lacking a reference (or matching a pattern like "R?").
// Matches KiCad's default annotator: number left-to-right, then
// top-to-bottom, per prefix, starting at 1 by default.
namespace annotator {

struct Options {
    // Restart numbering per prefix (true) or continue across the
    // whole project (false).
    bool per_sheet_reset = true;
    // Starting index for the prefix. KiCad uses 1.
    int  start_at        = 1;
    // Treat "R?", "R", or "R0" (any non-digit-terminated ref with
    // this prefix) as "unannotated". If false, only "R?" counts.
    bool loose_unannotated = true;
};

struct Change {
    kicad_model::UUID uuid;    // which SchSymbol
    std::string       old_ref; // "R?"
    std::string       new_ref; // "R7"
};

struct Result {
    std::vector<Change> changes;
    // Warnings (e.g. duplicate refs already present, refs skipped).
    std::vector<std::string> warnings;
};

// Assign fresh refs on the schematic in place. Mutates the schematic.
Result annotate(kicad_model::Schematic & sch, const Options & opts = {});

// Read the prefix from "R?" -> "R", "U12" -> "U".
std::string prefix_of(std::string_view ref);

// Parse the numeric tail of "R42" -> 42. Returns 0 when missing.
int number_of(std::string_view ref);

}
