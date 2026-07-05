// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/368_ibis/ibis.hpp"
#include "../modules/380_ibis_timing/ibis_timing.hpp"

namespace {

testing::TestOutcome run() {
    const char * sample = R"IBIS(
[IBIS Ver] 5.1
[Component] Chip
[Model] IO_3V3
Model_type   I/O
C_comp       2.0pF
Vinh 2.0
Vinl 0.8

[Pullup]
0   0.02
1   0.015
2   0.010
3   0.005

[Rising Waveform]
 0    0
 1n   0.5
 2n   1.5
 5n   3.3

[End]
)IBIS";
    auto f = ibis::parse(sample);
    if (!f) return testing::fail("parse");
    auto m = ibis_timing::evaluate(*f, "IO_3V3");
    if (m.vinh != 2.0) return testing::fail("vinh");
    if (m.rise_time_ns <= 0) return testing::fail("rise time");
    if (m.drive_impedance_ohm <= 0) return testing::fail("drive impedance");
    return testing::ok();
}

const int _r = testing::register_test(
    "ibis_timing",
    "IBIS timing: rise 10-90%, drive impedance from Pullup, VIH/VIL/C_comp.",
    &run);

} // namespace
