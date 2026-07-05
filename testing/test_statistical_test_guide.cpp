// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1423_statistical_test_guide/statistical_test_guide.hpp"

namespace {

testing::TestOutcome run_statistical_test_guide() {
    statistical_test_guide::init();
    auto s = statistical_test_guide::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    statistical_test_guide::shutdown();
    return testing::ok();
}

const int _reg_statistical_test_guide = testing::register_test(
    "statistical_test_guide",
    "1423_statistical_test_guide: stub status check",
    &run_statistical_test_guide);

}
