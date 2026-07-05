// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/869_gerber_and_fab_output_viewer.

#include "test_runner.hpp"
#include "../modules/340_kicad_bridge/kicad_bridge.hpp"
#include "../modules/869_gerber_and_fab_output_viewer/gerber_and_fab_output_viewer.hpp"

#include <cstdlib>
#include <string>
#include <unistd.h>

namespace {

std::string microwave_demo_pcb() {
    const char * candidates[] = {
        "kicad/demos/microwave/microwave.kicad_pcb",
        "../kicad/demos/microwave/microwave.kicad_pcb",
    };
    for (const char * c : candidates) if (::access(c, R_OK) == 0) return c;
    return {};
}

testing::TestOutcome run() {
    gerber_and_fab_output_viewer::init();

    auto specs = gerber_and_fab_output_viewer::default_layers_2layer();
    if (specs.empty()) return testing::fail("no default layers defined");
    // Every spec should have a canonical KiCad name.
    for (const auto & L : specs) {
        if (L.name.empty()) return testing::fail("empty layer name in default set");
    }

    kicad_bridge::init();
    if (!kicad_bridge::config().available)
        return testing::skip("kicad-cli not available");

    std::string pcb = microwave_demo_pcb();
    if (pcb.empty()) return testing::skip("microwave demo missing");

    char tmpl[] = "/tmp/tool_gerber_XXXXXX";
    if (!::mkdtemp(tmpl)) return testing::fail("mkdtemp failed");
    auto r = gerber_and_fab_output_viewer::render(pcb, tmpl,
        {specs[0], specs[8]}); // F.Cu + Edge.Cuts only, keep the test fast.

    if (!r.ok) return testing::fail("render returned no output: " + r.log);
    if (r.svg_paths.empty() && r.combined_path.empty())
        return testing::fail("no SVG files produced: " + r.log);

    gerber_and_fab_output_viewer::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "gerber_viewer",
    "Render two layers of the microwave demo to SVG via kicad-cli.",
    &run);

} // namespace
