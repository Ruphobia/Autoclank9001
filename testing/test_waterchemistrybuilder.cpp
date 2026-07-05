// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1823_waterchemistrybuilder/waterchemistrybuilder.hpp"

namespace {

testing::TestOutcome run_waterchemistrybuilder() {
    waterchemistrybuilder::init();
    auto s = waterchemistrybuilder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    waterchemistrybuilder::shutdown();
    return testing::ok();
}

const int _reg_waterchemistrybuilder = testing::register_test(
    "waterchemistrybuilder",
    "1823_waterchemistrybuilder: stub status check",
    &run_waterchemistrybuilder);

}
