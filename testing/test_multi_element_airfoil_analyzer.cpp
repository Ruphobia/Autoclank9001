// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1161_multi_element_airfoil_analyzer/multi_element_airfoil_analyzer.hpp"

namespace {

testing::TestOutcome run_multi_element_airfoil_analyzer() {
    multi_element_airfoil_analyzer::init();
    auto s = multi_element_airfoil_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multi_element_airfoil_analyzer::shutdown();
    return testing::ok();
}

const int _reg_multi_element_airfoil_analyzer = testing::register_test(
    "multi_element_airfoil_analyzer",
    "1161_multi_element_airfoil_analyzer: stub status check",
    &run_multi_element_airfoil_analyzer);

}
