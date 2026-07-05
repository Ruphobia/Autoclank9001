// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/843_schematic_capture.

#include "test_runner.hpp"
#include "../modules/343_circuit_intent/circuit_intent.hpp"
#include "../modules/843_schematic_capture/schematic_capture.hpp"

#include <string>

namespace {

const char * kMini = R"({
  "meta": {"title":"emit test"},
  "power": {"nets":[{"name":"+9V","voltage":9},{"name":"GND","voltage":0}]},
  "parts": [
    {"ref":"R1","value":"1k","lib_hint":"Device:R","footprint_hint":"Resistor_SMD:R_0603_1608Metric"},
    {"ref":"C1","value":"10nF","lib_hint":"Device:C","footprint_hint":"Capacitor_SMD:C_0603_1608Metric"}
  ],
  "connections": [ ["R1.1","+9V"], ["R1.2","C1.1"], ["C1.2","GND"] ],
  "placement_hints": {"board": {"size_mm":[40,20], "layers":2}}
})";

testing::TestOutcome run() {
    schematic_capture::init();
    circuit_intent::Intent intent;
    std::string err;
    if (!circuit_intent::parse_json(kMini, intent, err)) return testing::fail("intent parse: " + err);

    schematic_capture::Options opts;
    opts.generator = "ac9_test";
    auto r = schematic_capture::from_intent(intent, opts);
    if (!r.ok) return testing::fail("emit failed");
    if (r.sch_text.empty()) return testing::fail("empty output");
    if (r.sch_text.find("(kicad_sch") == std::string::npos) return testing::fail("no kicad_sch header");
    if (r.sch_text.find("(lib_symbols") == std::string::npos) return testing::fail("no lib_symbols block");
    if (r.sch_text.find("R1") == std::string::npos) return testing::fail("ref R1 missing");
    if (r.sch_text.find("(sheet_instances") == std::string::npos) return testing::fail("no sheet_instances");
    int depth = 0; bool in_str = false;
    for (char c : r.sch_text) {
        if (in_str) { if (c == '"') in_str = false; continue; }
        if (c == '"') in_str = true;
        else if (c == '(') ++depth;
        else if (c == ')') --depth;
        if (depth < 0) return testing::fail("unbalanced parens (extra close)");
    }
    if (depth != 0) return testing::fail("unbalanced parens (unclosed)");

    schematic_capture::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "schematic_capture",
    "Emit a .kicad_sch from a mini intent; verify structure and paren balance.",
    &run);

} // namespace
