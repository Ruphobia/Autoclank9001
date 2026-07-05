// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/378_ltspice_import/ltspice_import.hpp"

namespace {

testing::TestOutcome run() {
    const char * asc =
        "Version 4\n"
        "SHEET 1 880 680\n"
        "WIRE 96 128 96 224\n"
        "WIRE 96 224 224 224\n"
        "SYMBOL res 96 96 R0\n"
        "SYMATTR InstName R1\n"
        "SYMATTR Value 10k\n"
        "SYMBOL cap 224 224 R0\n"
        "SYMATTR InstName C1\n"
        "SYMATTR Value 10n\n"
        "FLAG 96 128 in\n"
        "FLAG 224 320 GND\n";
    auto f = ltspice_import::parse(asc);
    if (!f)                         return testing::fail("parse");
    if (f->wires.size()   != 2)     return testing::fail("wire count");
    if (f->symbols.size() != 2)     return testing::fail("symbol count");
    if (f->flags.size()   != 2)     return testing::fail("flag count");
    if (f->symbols[0].inst_name != "R1") return testing::fail("R1 name");
    if (f->symbols[0].value    != "10k") return testing::fail("R1 value");
    return testing::ok();
}

const int _r = testing::register_test(
    "ltspice_import",
    "LTspice .asc: Version/SHEET/WIRE/SYMBOL/SYMATTR/FLAG record parse.",
    &run);

} // namespace
