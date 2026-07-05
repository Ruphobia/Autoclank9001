// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1273_pv_array_layout_designer/pv_array_layout_designer.hpp"

namespace {

testing::TestOutcome run_pv_array_layout_designer() {
    pv_array_layout_designer::init();
    auto s = pv_array_layout_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pv_array_layout_designer::shutdown();
    return testing::ok();
}

const int _reg_pv_array_layout_designer = testing::register_test(
    "pv_array_layout_designer",
    "1273_pv_array_layout_designer: stub status check",
    &run_pv_array_layout_designer);

}
