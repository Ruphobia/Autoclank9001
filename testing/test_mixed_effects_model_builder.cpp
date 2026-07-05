// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1429_mixed_effects_model_builder/mixed_effects_model_builder.hpp"

namespace {

testing::TestOutcome run_mixed_effects_model_builder() {
    mixed_effects_model_builder::init();
    auto s = mixed_effects_model_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mixed_effects_model_builder::shutdown();
    return testing::ok();
}

const int _reg_mixed_effects_model_builder = testing::register_test(
    "mixed_effects_model_builder",
    "1429_mixed_effects_model_builder: stub status check",
    &run_mixed_effects_model_builder);

}
