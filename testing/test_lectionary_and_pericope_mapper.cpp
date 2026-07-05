// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/730_lectionary_and_pericope_mapper/lectionary_and_pericope_mapper.hpp"

namespace {

testing::TestOutcome run_lectionary_and_pericope_mapper() {
    lectionary_and_pericope_mapper::init();
    auto s = lectionary_and_pericope_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lectionary_and_pericope_mapper::shutdown();
    return testing::ok();
}

const int _reg_lectionary_and_pericope_mapper = testing::register_test(
    "lectionary_and_pericope_mapper",
    "730_lectionary_and_pericope_mapper: stub status check",
    &run_lectionary_and_pericope_mapper);

}
