// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1588_cross_section_builder/cross_section_builder.hpp"

namespace {

testing::TestOutcome run_cross_section_builder() {
    cross_section_builder::init();
    auto s = cross_section_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cross_section_builder::shutdown();
    return testing::ok();
}

const int _reg_cross_section_builder = testing::register_test(
    "cross_section_builder",
    "1588_cross_section_builder: stub status check",
    &run_cross_section_builder);

}
