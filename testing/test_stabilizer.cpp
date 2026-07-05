// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1648_stabilizer/stabilizer.hpp"

namespace {

testing::TestOutcome run_stabilizer() {
    stabilizer::init();
    auto s = stabilizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stabilizer::shutdown();
    return testing::ok();
}

const int _reg_stabilizer = testing::register_test(
    "stabilizer",
    "1648_stabilizer: stub status check",
    &run_stabilizer);

}
