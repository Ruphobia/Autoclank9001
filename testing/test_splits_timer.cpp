// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1984_splits_timer/splits_timer.hpp"

namespace {

testing::TestOutcome run_splits_timer() {
    splits_timer::init();
    auto s = splits_timer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    splits_timer::shutdown();
    return testing::ok();
}

const int _reg_splits_timer = testing::register_test(
    "splits_timer",
    "1984_splits_timer: stub status check",
    &run_splits_timer);

}
