// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/374_orcad_import/orcad_import.hpp"

namespace {

testing::TestOutcome run() {
    const char * kNet =
        "NETLIST DESCRIPTION;\n"
        "{ OrCAD-style legacy netlist }\n"
        "COMPONENTS\n"
        "'R1' 'RES_0603' 'R'\n"
        "'R2' 'RES_0603' 'R'\n"
        "'C1' 'CAP_0603' 'C'\n"
        "\n"
        "NETS\n"
        "'GND' : 'R1'.'2' 'C1'.'2'\n"
        "'+5V' : 'R1'.'1' 'R2'.'1'\n"
        "'NET1' : 'R2'.'2' 'C1'.'1'\n"
        "END.\n";

    auto f = orcad_import::parse(kNet);
    if (!f)                        return testing::fail("parse");
    if (f->components.size() != 3) return testing::fail("comp count " + std::to_string(f->components.size()));
    if (f->nets.size() != 3)       return testing::fail("net count " + std::to_string(f->nets.size()));

    bool saw_gnd = false;
    for (const auto & n : f->nets) if (n.name == "GND") {
        saw_gnd = true;
        if (n.pins.size() != 2) return testing::fail("GND pin count");
    }
    if (!saw_gnd) return testing::fail("no GND net");
    return testing::ok();
}

const int _r = testing::register_test(
    "orcad_import",
    "OrCAD legacy netlist parser: COMPONENTS + NETS sections, quoted refs, ref.pin pairs.",
    &run);

} // namespace
