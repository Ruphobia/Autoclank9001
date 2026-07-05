// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2117_electrical_load_profile_builder/electrical_load_profile_builder.hpp"

namespace {

testing::TestOutcome run_electrical_load_profile_builder() {
    electrical_load_profile_builder::init();
    auto s = electrical_load_profile_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    electrical_load_profile_builder::shutdown();
    return testing::ok();
}

const int _reg_electrical_load_profile_builder = testing::register_test(
    "electrical_load_profile_builder",
    "2117_electrical_load_profile_builder: stub status check",
    &run_electrical_load_profile_builder);

}
