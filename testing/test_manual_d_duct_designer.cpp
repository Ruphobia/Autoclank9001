// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2024_manual_d_duct_designer/manual_d_duct_designer.hpp"

namespace {

testing::TestOutcome run_manual_d_duct_designer() {
    manual_d_duct_designer::init();
    auto s = manual_d_duct_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    manual_d_duct_designer::shutdown();
    return testing::ok();
}

const int _reg_manual_d_duct_designer = testing::register_test(
    "manual_d_duct_designer",
    "2024_manual_d_duct_designer: stub status check",
    &run_manual_d_duct_designer);

}
