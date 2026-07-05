// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Netlist derivation from an in-memory Schematic. Equivalent scope to
// KiCad's CONNECTION_GRAPH: walk wires + labels + pins + junctions,
// union-find connected endpoints, name the resulting nets from any
// labels attached, output an ordered netlist.
//
// Coordinate model: all positions are integer nanometers. Two points
// belong to the same net iff (a) they are exactly equal, or (b) they
// lie on the same wire segment (endpoint-to-endpoint or intermediate
// via a junction).
namespace netlist {

struct Pin {
    std::string ref;         // "U1"
    std::string number;      // "1"
    std::string electrical;  // "input","output","bidirectional","passive",
                             // "power_in","power_out","tri_state",
                             // "unspecified","open_collector","open_emitter",
                             // "no_connect","unconnected"
};

struct Net {
    int         id       = 0;
    std::string name;        // "" for unnamed local nets, else label name
    std::vector<Pin> pins;
};

struct Netlist {
    std::vector<Net> nets;   // id 0 is reserved for the "no-connect" net
    // Diagnostics from derivation (unresolved pins, ambiguous names).
    std::vector<std::string> warnings;
};

// Derive a netlist from a Schematic. The Schematic must have its
// lib_symbols populated so pin positions on placed SchSymbols can be
// computed. Uses only the root sheet in this pass; hierarchical
// sheets are a follow-up.
Netlist derive(const kicad_model::Schematic & sch);

// Emit a KiCad "kicadsexpr" style netlist. This is the format
// eeschema-cli emits with `sch export netlist --format kicadsexpr`
// and pcbnew reads via the netlist importer.
std::string to_kicad_netlist(const Netlist & nl,
                             const kicad_model::Schematic & sch);

// Emit a SPICE netlist. Includes .title/.end wrappers.
std::string to_spice_netlist(const Netlist & nl,
                             const kicad_model::Schematic & sch,
                             std::string_view analysis = {});

} // namespace netlist
