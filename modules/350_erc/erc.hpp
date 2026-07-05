// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"
#include "../349_netlist/netlist.hpp"

#include <string>
#include <string_view>
#include <vector>

// ERC (Electrical Rules Check) engine.
//
// Consumes a Schematic + its derived Netlist. Emits a list of
// violations compatible with the shape kicad_bridge::Report expects
// so we can flow them through the same viewer.
//
// Rules covered in this pass:
//   * duplicate reference designators
//   * pin-to-pin electrical compatibility (12x12 KiCad-style matrix)
//   * pin not connected (net has exactly one pin and no explicit no_connect)
//   * pin not driven (input pins on a net with no driver)
//   * power pin not driven (power_in on a net with no power_out)
//   * unresolved lib_symbol (SchSymbol pointing at a lib_id not in
//     Schematic.lib_symbols)
//   * dangling label (label attached to a point with no wire)
//   * missing Reference / Value field
//
// Severity levels default to KiCad's defaults; callers may override
// via Config.
namespace erc {

enum class Severity : int { Error = 0, Warning = 1, Info = 2, Ignore = 3 };

struct Violation {
    std::string rule_id;         // "duplicate_reference","pin_to_pin", ...
    Severity    severity = Severity::Error;
    std::string message;         // human-readable
    // Optional geometry: where the offense happened.
    bool           has_pos = false;
    geom::VECTOR2I at{0,0};
    // References involved (component / net).
    std::vector<std::string> component_refs;
    std::string   net_name;
};

struct Report {
    std::vector<Violation> violations;
    std::size_t errors   = 0;
    std::size_t warnings = 0;
    std::size_t infos    = 0;
    std::size_t ignored  = 0;
};

// Standard KiCad pin electrical categories, indexed 0..11 matching
// KiCad's own 12x12 pin_map.
enum PinType : int {
    PIN_INPUT           = 0,
    PIN_OUTPUT          = 1,
    PIN_BIDIRECTIONAL   = 2,
    PIN_TRISTATE        = 3,
    PIN_PASSIVE         = 4,
    PIN_UNSPECIFIED     = 5,
    PIN_POWER_IN        = 6,
    PIN_POWER_OUT       = 7,
    PIN_OPEN_COLLECTOR  = 8,
    PIN_OPEN_EMITTER    = 9,
    PIN_NO_CONNECT      = 10,
    PIN_UNCONNECTED     = 11
};

PinType classify_pin(std::string_view electrical);

// Compatibility level between two pin types.
//   0 = OK
//   1 = warning
//   2 = error
int pin_compatibility(PinType a, PinType b);

struct Config {
    Severity duplicate_reference     = Severity::Error;
    Severity pin_to_pin              = Severity::Warning;
    Severity pin_not_connected       = Severity::Error;
    Severity pin_not_driven          = Severity::Error;
    Severity power_pin_not_driven    = Severity::Error;
    Severity unresolved_lib_symbol   = Severity::Warning;
    Severity dangling_label          = Severity::Warning;
    Severity missing_reference       = Severity::Error;
    Severity missing_value           = Severity::Warning;
};

// Run every enabled rule and return the aggregate report.
Report run(const kicad_model::Schematic & sch,
           const netlist::Netlist        & nl,
           const Config                  & cfg = {});

// Emit the report in the same JSON envelope kicad-cli's `sch erc
// --format json` produces so the existing UI can consume it directly.
std::string to_kicad_json(const Report & rep, std::string_view source_path);

} // namespace erc
