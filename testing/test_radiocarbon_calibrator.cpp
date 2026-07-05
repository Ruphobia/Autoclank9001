// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1522_radiocarbon_calibrator/radiocarbon_calibrator.hpp"

namespace {

testing::TestOutcome run_radiocarbon_calibrator() {
    radiocarbon_calibrator::init();
    auto s = radiocarbon_calibrator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    radiocarbon_calibrator::shutdown();
    return testing::ok();
}

const int _reg_radiocarbon_calibrator = testing::register_test(
    "radiocarbon_calibrator",
    "1522_radiocarbon_calibrator: stub status check",
    &run_radiocarbon_calibrator);

}
