// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/168_push_notification_tester/push_notification_tester.hpp"

namespace {

testing::TestOutcome run_push_notification_tester() {
    push_notification_tester::init();
    auto s = push_notification_tester::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    push_notification_tester::shutdown();
    return testing::ok();
}

const int _reg_push_notification_tester = testing::register_test(
    "push_notification_tester",
    "168_push_notification_tester: stub status check",
    &run_push_notification_tester);

}
