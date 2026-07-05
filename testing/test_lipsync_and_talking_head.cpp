// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1645_lipsync_and_talking_head/lipsync_and_talking_head.hpp"

namespace {

testing::TestOutcome run_lipsync_and_talking_head() {
    lipsync_and_talking_head::init();
    auto s = lipsync_and_talking_head::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lipsync_and_talking_head::shutdown();
    return testing::ok();
}

const int _reg_lipsync_and_talking_head = testing::register_test(
    "lipsync_and_talking_head",
    "1645_lipsync_and_talking_head: stub status check",
    &run_lipsync_and_talking_head);

}
