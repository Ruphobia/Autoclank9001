// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1421_likert_scale_builder/likert_scale_builder.hpp"

namespace {

testing::TestOutcome run_likert_scale_builder() {
    likert_scale_builder::init();
    auto s = likert_scale_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    likert_scale_builder::shutdown();
    return testing::ok();
}

const int _reg_likert_scale_builder = testing::register_test(
    "likert_scale_builder",
    "1421_likert_scale_builder: stub status check",
    &run_likert_scale_builder);

}
