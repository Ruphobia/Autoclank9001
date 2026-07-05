// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1143_constellation_designer/constellation_designer.hpp"

namespace {

testing::TestOutcome run_constellation_designer() {
    constellation_designer::init();
    auto s = constellation_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    constellation_designer::shutdown();
    return testing::ok();
}

const int _reg_constellation_designer = testing::register_test(
    "constellation_designer",
    "1143_constellation_designer: stub status check",
    &run_constellation_designer);

}
