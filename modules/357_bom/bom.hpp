// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <vector>

// BOM (Bill of Materials) generation from a Schematic.
// Groups symbols by (Value, Footprint, Manufacturer, MPN) and lists
// their references; supports arbitrary custom columns via field names.
namespace bom {

struct Options {
    // Columns to include, in order. Defaults to KiCad's canonical set
    // when empty.
    std::vector<std::string> columns;
    // How to combine multiple references into one cell: "list"
    // (R1,R2,R3) or "range" (R1-R3,R7).
    std::string ref_combine = "list";
    // Exclude symbols with DNP set.
    bool exclude_dnp = false;
    // Exclude "power" or "#PWR"-prefixed pseudo-parts.
    bool exclude_power_symbols = true;
};

struct Row {
    int         count = 0;
    std::vector<std::string> refs;
    // Column values keyed by column name.
    std::vector<std::pair<std::string, std::string>> cells;
};

struct BOM {
    std::vector<std::string> columns;     // final column list
    std::vector<Row>         rows;
};

BOM generate(const kicad_model::Schematic & sch, const Options & opts = {});

// Emit as CSV (with header).
std::string to_csv (const BOM & b);
// Emit as HTML table (self-contained; embeddable in the UI).
std::string to_html(const BOM & b);
// Emit as JSON (rows + columns).
std::string to_json(const BOM & b);

}
