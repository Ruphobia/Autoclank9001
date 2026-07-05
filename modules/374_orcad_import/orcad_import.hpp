// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// OrCAD PCB Designer legacy netlist ("PCB2" and .net) parser.
//
// The .net format (also emitted by KiCad's "legacy OrCAD" netlist
// export) is line-oriented ASCII:
//
//   NETLIST DESCRIPTION;
//   { OrCAD ... }
//   COMPONENTS
//     'R1' 'RES_0603' 'R'
//     'C1' 'CAP_0603' 'C'
//   ...
//   NETS
//     'GND' : 'R1'.'2' 'C1'.'2'
//     '+5V' : 'R1'.'1'
//   ...
//   END.
//
// This parser returns a compact model of components and their nets
// which downstream code can map into the ac9 Board (via footprint
// lookup) or emit into a KiCad-style netlist.
namespace orcad_import {

struct Component {
    std::string ref;
    std::string package;      // "RES_0603" etc.
    std::string value;
};

struct Net {
    std::string name;
    struct Pin { std::string ref, pin; };
    std::vector<Pin> pins;
};

struct File {
    std::vector<Component> components;
    std::vector<Net>       nets;
    std::vector<std::string> warnings;
};

std::optional<File> parse(std::string_view text);

}
