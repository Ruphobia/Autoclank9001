// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/377_altium_import/altium_import.hpp"

namespace {

testing::TestOutcome run() {
    const char * kAscii =
        "PCB FILE VERSION 3.00\n"
        "TRACK X1=100 Y1=100 X2=500 Y2=100 W=10 L=1 NET=+5V\n"
        "TRACK X1=500 Y1=100 X2=500 Y2=500 W=10 L=1 NET=+5V\n"
        "VIA X=500 Y=100 D=25 H=12 NET=+5V\n"
        "COMPONENT X=800 Y=800 L=1 ROT=90 REF=R1 VALUE=10k PACKAGE=0603\n"
        "FILL X1=0 Y1=0 X2=100 Y2=100 L=1\n";

    altium_import::ImportReport rep;
    auto b = altium_import::read_board(kAscii, &rep);
    if (!b) return testing::fail("read");
    if (!rep.ok) return testing::fail("no header seen");
    if (rep.tracks     != 2) return testing::fail("tracks " + std::to_string(rep.tracks));
    if (rep.vias       != 1) return testing::fail("vias " + std::to_string(rep.vias));
    if (rep.components != 1) return testing::fail("components " + std::to_string(rep.components));
    if (rep.fills      != 1) return testing::fail("fills " + std::to_string(rep.fills));

    // Verify net was interned.
    bool saw_5v = false;
    for (const auto & n : b->nets) if (n.name == "+5V") { saw_5v = true; break; }
    if (!saw_5v) return testing::fail("+5V not interned");
    return testing::ok();
}

const int _r = testing::register_test(
    "altium_import",
    "Altium/Protel ASCII .PCB parse: TRACK/VIA/COMPONENT/FILL records, mils->mm, layer numeric mapping.",
    &run);

} // namespace
