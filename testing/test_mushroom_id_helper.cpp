// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2122_mushroom_id_helper/mushroom_id_helper.hpp"

namespace {

testing::TestOutcome run_mushroom_id_helper() {
    mushroom_id_helper::init();
    auto s = mushroom_id_helper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mushroom_id_helper::shutdown();
    return testing::ok();
}

const int _reg_mushroom_id_helper = testing::register_test(
    "mushroom_id_helper",
    "2122_mushroom_id_helper: stub status check",
    &run_mushroom_id_helper);

}
