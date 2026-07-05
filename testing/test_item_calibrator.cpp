// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/523_item_calibrator/item_calibrator.hpp"

namespace {

testing::TestOutcome run_item_calibrator() {
    item_calibrator::init();
    auto s = item_calibrator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    item_calibrator::shutdown();
    return testing::ok();
}

const int _reg_item_calibrator = testing::register_test(
    "item_calibrator",
    "523_item_calibrator: stub status check",
    &run_item_calibrator);

}
