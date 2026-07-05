// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1888_die_marriage_and_plate_mapper/die_marriage_and_plate_mapper.hpp"

namespace {

testing::TestOutcome run_die_marriage_and_plate_mapper() {
    die_marriage_and_plate_mapper::init();
    auto s = die_marriage_and_plate_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    die_marriage_and_plate_mapper::shutdown();
    return testing::ok();
}

const int _reg_die_marriage_and_plate_mapper = testing::register_test(
    "die_marriage_and_plate_mapper",
    "1888_die_marriage_and_plate_mapper: stub status check",
    &run_die_marriage_and_plate_mapper);

}
