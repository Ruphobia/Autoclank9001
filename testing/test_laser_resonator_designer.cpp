// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/878_laser_resonator_designer/laser_resonator_designer.hpp"

namespace {

testing::TestOutcome run_laser_resonator_designer() {
    laser_resonator_designer::init();
    auto s = laser_resonator_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    laser_resonator_designer::shutdown();
    return testing::ok();
}

const int _reg_laser_resonator_designer = testing::register_test(
    "laser_resonator_designer",
    "878_laser_resonator_designer: stub status check",
    &run_laser_resonator_designer);

}
