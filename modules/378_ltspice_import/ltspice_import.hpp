// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// LTspice .asc schematic + .net netlist reader.
//
// LTspice's ASC files are keyword-per-line ASCII:
//   Version 4
//   SHEET 1 880 680
//   WIRE 96 128 96 224
//   SYMBOL res 96 96 R0
//   SYMATTR InstName R1
//   SYMATTR Value 10k
//   FLAG 96 224 GND
//
// This parser exposes a light data model that downstream code can map
// into ac9 SchSymbol / SchWire / SchLabel items.
namespace ltspice_import {

struct Wire {
    long long x1 = 0, y1 = 0, x2 = 0, y2 = 0;
};

struct Symbol {
    std::string type;      // "res", "cap", "voltage", ...
    long long   x = 0, y = 0;
    std::string rotation;  // "R0"|"R90"|"R180"|"R270"|"M0"|"M90"|...
    std::string inst_name;
    std::string value;
    std::vector<std::pair<std::string, std::string>> attrs;
};

struct Flag {
    long long   x = 0, y = 0;
    std::string name;
};

struct File {
    int version = 4;
    long long sheet_w = 0, sheet_h = 0;
    std::vector<Wire>   wires;
    std::vector<Symbol> symbols;
    std::vector<Flag>   flags;
    std::vector<std::string> warnings;
};

std::optional<File> parse(std::string_view text);

}
