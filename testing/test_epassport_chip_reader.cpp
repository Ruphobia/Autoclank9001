// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/603_epassport_chip_reader/epassport_chip_reader.hpp"

namespace {

testing::TestOutcome run_epassport_chip_reader() {
    epassport_chip_reader::init();
    auto s = epassport_chip_reader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    epassport_chip_reader::shutdown();
    return testing::ok();
}

const int _reg_epassport_chip_reader = testing::register_test(
    "epassport_chip_reader",
    "603_epassport_chip_reader: stub status check",
    &run_epassport_chip_reader);

}
