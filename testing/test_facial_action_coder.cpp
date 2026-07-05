// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1434_facial_action_coder/facial_action_coder.hpp"

namespace {

testing::TestOutcome run_facial_action_coder() {
    facial_action_coder::init();
    auto s = facial_action_coder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    facial_action_coder::shutdown();
    return testing::ok();
}

const int _reg_facial_action_coder = testing::register_test(
    "facial_action_coder",
    "1434_facial_action_coder: stub status check",
    &run_facial_action_coder);

}
