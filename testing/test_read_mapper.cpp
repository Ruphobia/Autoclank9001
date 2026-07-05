// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1351_read_mapper/read_mapper.hpp"

namespace {

testing::TestOutcome run_read_mapper() {
    read_mapper::init();
    auto s = read_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    read_mapper::shutdown();
    return testing::ok();
}

const int _reg_read_mapper = testing::register_test(
    "read_mapper",
    "1351_read_mapper: stub status check",
    &run_read_mapper);

}
