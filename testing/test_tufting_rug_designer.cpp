// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1780_tufting_rug_designer/tufting_rug_designer.hpp"

namespace {

testing::TestOutcome run_tufting_rug_designer() {
    tufting_rug_designer::init();
    auto s = tufting_rug_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tufting_rug_designer::shutdown();
    return testing::ok();
}

const int _reg_tufting_rug_designer = testing::register_test(
    "tufting_rug_designer",
    "1780_tufting_rug_designer: stub status check",
    &run_tufting_rug_designer);

}
