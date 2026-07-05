// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/368_ibis/ibis.hpp"

namespace {

testing::TestOutcome run() {
    const char * kSample = R"IBIS(
|+---------------------------------------------------------------
| Sample IBIS file
[IBIS Ver]   5.1
[File Name]  sample.ibs
[File Rev]   1.0
[Date]       2026-07-05
[Source]     synthetic
[Notes]      test fixture

[Component]  MyChip_100
[Manufacturer] AutoClank
[Package]
R_pkg     0.5     0.4     0.6
L_pkg     3.0nH   2.5nH   3.5nH
C_pkg     0.5pF   0.4pF   0.6pF

[Pin]  signal_name    model_name   R_pin   L_pin   C_pin
1      VDD            POWER
2      GND            GND
3      GPIO0          IO_3V3       0.5     3n      0.5p
4      GPIO1          IO_3V3       0.5     3n      0.5p

[Model]  IO_3V3
Model_type   I/O
C_comp       2.0pF
Vinh         2.0
Vinl         0.8

[Pulldown]
| V     I
-1.0    -10m
 0.0     0
 1.0     5m
 3.3     20m

[Rising Waveform]
| t    V
 0      0
 1n     0.5
 5n     3.3

[End]
)IBIS";

    auto f = ibis::parse(kSample);
    if (!f) return testing::fail("parse failed");
    if (f->header.ibis_ver != "5.1")   return testing::fail("ibis_ver");
    if (f->header.component != "MyChip_100") return testing::fail("component");
    if (f->pins.size() < 3)             return testing::fail("pins");
    bool saw_gpio = false;
    for (const auto & p : f->pins) if (p.signal_name == "GPIO0") { saw_gpio = true; break; }
    if (!saw_gpio) return testing::fail("gpio pin");

    if (f->models.size() != 1) return testing::fail("models");
    if (f->models[0].model_type != "I/O") return testing::fail("model type");
    if (f->models[0].iv.empty()) return testing::fail("iv");
    if (f->models[0].vt.empty()) return testing::fail("vt");
    if (f->models[0].vt[0].points.size() != 3) return testing::fail("vt points");
    return testing::ok();
}

const int _r = testing::register_test(
    "ibis",
    "IBIS parser: [IBIS Ver]/[Component]/[Pin]/[Model]/[Pulldown]/[Rising Waveform] with engineering suffixes.",
    &run);

} // namespace
