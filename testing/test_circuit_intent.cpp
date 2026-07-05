// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/343_circuit_intent.

#include "test_runner.hpp"
#include "../modules/343_circuit_intent/circuit_intent.hpp"

#include <string>

namespace {

const char * kMiniIntent = R"({
  "meta": {"title":"555 blinker","notes":"1 Hz LED, 9V"},
  "power": {"nets":[{"name":"+9V","voltage":9},{"name":"GND","voltage":0}]},
  "parts": [
    {"ref":"U1","value":"NE555","lib_hint":"Timer:NE555","footprint_hint":"Package_DIP:DIP-8_W7.62mm"},
    {"ref":"R1","value":"33k",  "lib_hint":"Device:R",   "footprint_hint":"Resistor_THT:R_Axial"},
    {"ref":"D1","value":"LED",  "lib_hint":"Device:LED", "footprint_hint":"LED_THT:LED_D5.0mm"}
  ],
  "connections": [
    ["U1.VCC","+9V"], ["U1.GND","GND"], ["U1.OUT","D1.A"], ["D1.K","GND"], ["R1.1","+9V"], ["R1.2","U1.DIS"]
  ],
  "placement_hints": {"board": {"size_mm":[50,30],"layers":2}}
})";

testing::TestOutcome run() {
    circuit_intent::Intent in;
    std::string err;
    if (!circuit_intent::parse_json(kMiniIntent, in, err))
        return testing::fail("parse: " + err);

    if (in.title != "555 blinker") return testing::fail("title lost");
    if (in.power_nets.size() != 2) return testing::fail("power_nets count");
    if (in.parts.size() != 3)      return testing::fail("parts count");
    if (in.nets.empty())           return testing::fail("connections did not become nets");
    if (in.board.width_mm != 50 || in.board.height_mm != 30)
        return testing::fail("board dims lost");

    auto diags = circuit_intent::validate(in);
    for (const auto & d : diags) {
        if (d.severity == circuit_intent::Diagnostic::Severity::Error)
            return testing::fail("unexpected validation error: " + d.field + " " + d.message);
    }

    // Round-trip through JSON.
    std::string j = circuit_intent::to_json(in, 0);
    circuit_intent::Intent in2;
    if (!circuit_intent::parse_json(j, in2, err))
        return testing::fail("re-parse: " + err);
    if (in2.parts.size() != in.parts.size()) return testing::fail("parts count changed on round-trip");
    if (in2.nets.size()  != in.nets.size())  return testing::fail("nets count changed on round-trip");

    // Duplicate-ref error must surface.
    in2.parts.push_back(in2.parts[0]);
    auto d2 = circuit_intent::validate(in2);
    bool saw_dup = false;
    for (const auto & d : d2) {
        if (d.message.find("duplicate") != std::string::npos) { saw_dup = true; break; }
    }
    if (!saw_dup) return testing::fail("duplicate ref not detected");

    return testing::ok();
}

const int _r = testing::register_test(
    "circuit_intent",
    "Parse a small circuit intent JSON, validate it, round-trip, and catch a duplicate ref.",
    &run);

} // namespace
