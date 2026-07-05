// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/844_pcb_layout.

#include "test_runner.hpp"
#include "../modules/343_circuit_intent/circuit_intent.hpp"
#include "../modules/844_pcb_layout/pcb_layout.hpp"

#include <string>

namespace {

const char * kMini = R"({
  "meta": {"title":"emit test"},
  "power": {"nets":[{"name":"+3V3","voltage":3.3},{"name":"GND","voltage":0}]},
  "parts": [
    {"ref":"R1","value":"1k","lib_hint":"Device:R","footprint_hint":"Resistor_SMD:R_0603_1608Metric"},
    {"ref":"C1","value":"10nF","lib_hint":"Device:C","footprint_hint":"Capacitor_SMD:C_0603_1608Metric"}
  ],
  "connections": [ ["R1.1","+3V3"], ["R1.2","C1.1"], ["C1.2","GND"] ],
  "placement_hints": {"board": {"size_mm":[35,20], "layers":2}}
})";

testing::TestOutcome run() {
    pcb_layout::init();
    circuit_intent::Intent intent;
    std::string err;
    if (!circuit_intent::parse_json(kMini, intent, err)) return testing::fail("intent parse: " + err);

    auto r = pcb_layout::from_intent(intent, {});
    if (!r.ok) return testing::fail("emit failed");
    if (r.pcb_text.empty()) return testing::fail("empty output");
    if (r.pcb_text.find("(kicad_pcb") == std::string::npos) return testing::fail("no kicad_pcb header");
    if (r.pcb_text.find("(layers") == std::string::npos) return testing::fail("no layers block");
    if (r.pcb_text.find("(setup") == std::string::npos)  return testing::fail("no setup block");
    if (r.pcb_text.find("Edge.Cuts") == std::string::npos) return testing::fail("no board outline");
    if (r.emitted_nets < 3) return testing::fail("expected >= 3 nets");
    int depth = 0; bool in_str = false;
    for (char c : r.pcb_text) {
        if (in_str) { if (c == '"') in_str = false; continue; }
        if (c == '"') in_str = true;
        else if (c == '(') ++depth;
        else if (c == ')') --depth;
        if (depth < 0) return testing::fail("unbalanced parens (extra close)");
    }
    if (depth != 0) return testing::fail("unbalanced parens (unclosed)");

    pcb_layout::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "pcb_layout",
    "Emit a .kicad_pcb from a mini intent; verify structure, layers, nets, paren balance.",
    &run);

} // namespace
