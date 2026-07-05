// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1836_cobbling_and_shoe_repair/cobbling_and_shoe_repair.hpp"

namespace {

testing::TestOutcome run_cobbling_and_shoe_repair() {
    cobbling_and_shoe_repair::init();
    auto s = cobbling_and_shoe_repair::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cobbling_and_shoe_repair::shutdown();
    return testing::ok();
}

const int _reg_cobbling_and_shoe_repair = testing::register_test(
    "cobbling_and_shoe_repair",
    "1836_cobbling_and_shoe_repair: stub status check",
    &run_cobbling_and_shoe_repair);

}
