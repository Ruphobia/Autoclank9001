// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/248_lock_and_deadlock_inspector/lock_and_deadlock_inspector.hpp"

namespace {

testing::TestOutcome run_lock_and_deadlock_inspector() {
    lock_and_deadlock_inspector::init();
    auto s = lock_and_deadlock_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lock_and_deadlock_inspector::shutdown();
    return testing::ok();
}

const int _reg_lock_and_deadlock_inspector = testing::register_test(
    "lock_and_deadlock_inspector",
    "248_lock_and_deadlock_inspector: stub status check",
    &run_lock_and_deadlock_inspector);

}
