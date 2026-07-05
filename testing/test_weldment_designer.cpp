// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/942_weldment_designer/weldment_designer.hpp"

namespace {

testing::TestOutcome run_weldment_designer() {
    weldment_designer::init();
    auto s = weldment_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    weldment_designer::shutdown();
    return testing::ok();
}

const int _reg_weldment_designer = testing::register_test(
    "weldment_designer",
    "942_weldment_designer: stub status check",
    &run_weldment_designer);

}
