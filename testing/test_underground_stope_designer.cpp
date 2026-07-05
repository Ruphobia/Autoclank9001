// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1093_underground_stope_designer/underground_stope_designer.hpp"

namespace {

testing::TestOutcome run_underground_stope_designer() {
    underground_stope_designer::init();
    auto s = underground_stope_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    underground_stope_designer::shutdown();
    return testing::ok();
}

const int _reg_underground_stope_designer = testing::register_test(
    "underground_stope_designer",
    "1093_underground_stope_designer: stub status check",
    &run_underground_stope_designer);

}
