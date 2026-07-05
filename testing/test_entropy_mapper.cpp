// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/360_entropy_mapper/entropy_mapper.hpp"

namespace {

testing::TestOutcome run_entropy_mapper() {
    entropy_mapper::init();
    auto s = entropy_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    entropy_mapper::shutdown();
    return testing::ok();
}

const int _reg_entropy_mapper = testing::register_test(
    "entropy_mapper",
    "360_entropy_mapper: stub status check",
    &run_entropy_mapper);

}
