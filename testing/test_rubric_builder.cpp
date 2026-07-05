// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/517_rubric_builder/rubric_builder.hpp"

namespace {

testing::TestOutcome run_rubric_builder() {
    rubric_builder::init();
    auto s = rubric_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rubric_builder::shutdown();
    return testing::ok();
}

const int _reg_rubric_builder = testing::register_test(
    "rubric_builder",
    "517_rubric_builder: stub status check",
    &run_rubric_builder);

}
