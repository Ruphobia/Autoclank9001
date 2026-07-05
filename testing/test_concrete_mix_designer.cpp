// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1052_concrete_mix_designer/concrete_mix_designer.hpp"

namespace {

testing::TestOutcome run_concrete_mix_designer() {
    concrete_mix_designer::init();
    auto s = concrete_mix_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    concrete_mix_designer::shutdown();
    return testing::ok();
}

const int _reg_concrete_mix_designer = testing::register_test(
    "concrete_mix_designer",
    "1052_concrete_mix_designer: stub status check",
    &run_concrete_mix_designer);

}
