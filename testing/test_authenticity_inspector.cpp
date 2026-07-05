// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/602_authenticity_inspector/authenticity_inspector.hpp"

namespace {

testing::TestOutcome run_authenticity_inspector() {
    authenticity_inspector::init();
    auto s = authenticity_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    authenticity_inspector::shutdown();
    return testing::ok();
}

const int _reg_authenticity_inspector = testing::register_test(
    "authenticity_inspector",
    "602_authenticity_inspector: stub status check",
    &run_authenticity_inspector);

}
