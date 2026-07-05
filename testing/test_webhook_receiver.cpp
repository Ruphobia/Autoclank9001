// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/147_webhook_receiver/webhook_receiver.hpp"

namespace {

testing::TestOutcome run_webhook_receiver() {
    webhook_receiver::init();
    auto s = webhook_receiver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    webhook_receiver::shutdown();
    return testing::ok();
}

const int _reg_webhook_receiver = testing::register_test(
    "webhook_receiver",
    "147_webhook_receiver: stub status check",
    &run_webhook_receiver);

}
