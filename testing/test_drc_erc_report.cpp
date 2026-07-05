// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/340_kicad_bridge/drc_erc_report.

#include "test_runner.hpp"
#include "../modules/340_kicad_bridge/drc_erc_report.hpp"

#include <string>

namespace {

// Minimal shape of the JSON kicad-cli emits with --format json.
const char * kSample = R"({
  "$schema": "kicad_drc",
  "source": "board.kicad_pcb",
  "kicad_version": "10.0.4",
  "coordinate_units": "mm",
  "violations": [
    {
      "type": "clearance",
      "severity": "error",
      "description": "Clearance violation: 0.10mm < 0.15mm",
      "items": [ {"uuid":"aaaa", "description":"Track", "pos": {"x":12.34, "y":56.78}} ]
    },
    {
      "type": "silk_over_copper",
      "severity": "warning",
      "description": "Silk over copper on F.SilkS",
      "items": [ {"uuid":"bbbb", "description":"Text"} ]
    }
  ],
  "unconnected_items": [
    {
      "type": "unconnected_items",
      "severity": "error",
      "description": "R1 pad 2 is not connected to net Net-(R1-Pad2)",
      "items": []
    }
  ]
})";

testing::TestOutcome run() {
    auto rep = kicad_bridge::parse_report(kSample);
    if (!rep.ok) return testing::fail("parse: " + rep.parse_error);
    if (rep.kicad_version != "10.0.4") return testing::fail("kicad_version parsed wrong");
    if (rep.violations.size() != 2)    return testing::fail("expected 2 violations");
    if (rep.unconnected_items.size() != 1) return testing::fail("expected 1 unconnected");
    if (rep.errors   != 2)  return testing::fail("error count " + std::to_string(rep.errors));
    if (rep.warnings != 1)  return testing::fail("warning count " + std::to_string(rep.warnings));

    // First violation should have a position.
    if (!rep.violations[0].has_pos)      return testing::fail("first violation missing pos");
    if (rep.violations[0].x_mm  != 12.34) return testing::fail("x wrong");
    if (rep.violations[0].y_mm  != 56.78) return testing::fail("y wrong");

    std::string s = kicad_bridge::summarize(rep);
    if (s.find("errors") == std::string::npos) return testing::fail("summary missing 'errors'");

    return testing::ok();
}

const int _r = testing::register_test(
    "drc_erc_report",
    "Parse the JSON envelope kicad-cli erc/drc emits; verify counts, positions, and summary.",
    &run);

} // namespace
