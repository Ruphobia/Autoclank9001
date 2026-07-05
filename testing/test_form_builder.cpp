// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/128_form_builder/form_builder.hpp"

namespace {

testing::TestOutcome run_form_builder() {
    form_builder::init();
    auto s = form_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    form_builder::shutdown();
    return testing::ok();
}

const int _reg_form_builder = testing::register_test(
    "form_builder",
    "128_form_builder: stub status check",
    &run_form_builder);

}
