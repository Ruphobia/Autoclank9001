// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1050_stormwater_drainage_designer/stormwater_drainage_designer.hpp"

namespace {

testing::TestOutcome run_stormwater_drainage_designer() {
    stormwater_drainage_designer::init();
    auto s = stormwater_drainage_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stormwater_drainage_designer::shutdown();
    return testing::ok();
}

const int _reg_stormwater_drainage_designer = testing::register_test(
    "stormwater_drainage_designer",
    "1050_stormwater_drainage_designer: stub status check",
    &run_stormwater_drainage_designer);

}
