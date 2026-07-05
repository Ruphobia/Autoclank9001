// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1036_retaining_wall_designer/retaining_wall_designer.hpp"

namespace {

testing::TestOutcome run_retaining_wall_designer() {
    retaining_wall_designer::init();
    auto s = retaining_wall_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    retaining_wall_designer::shutdown();
    return testing::ok();
}

const int _reg_retaining_wall_designer = testing::register_test(
    "retaining_wall_designer",
    "1036_retaining_wall_designer: stub status check",
    &run_retaining_wall_designer);

}
