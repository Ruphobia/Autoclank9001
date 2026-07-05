// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/506_reverse_outliner/reverse_outliner.hpp"

namespace {

testing::TestOutcome run_reverse_outliner() {
    reverse_outliner::init();
    auto s = reverse_outliner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reverse_outliner::shutdown();
    return testing::ok();
}

const int _reg_reverse_outliner = testing::register_test(
    "reverse_outliner",
    "506_reverse_outliner: stub status check",
    &run_reverse_outliner);

}
