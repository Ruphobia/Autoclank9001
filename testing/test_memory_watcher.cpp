// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1992_memory_watcher/memory_watcher.hpp"

namespace {

testing::TestOutcome run_memory_watcher() {
    memory_watcher::init();
    auto s = memory_watcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    memory_watcher::shutdown();
    return testing::ok();
}

const int _reg_memory_watcher = testing::register_test(
    "memory_watcher",
    "1992_memory_watcher: stub status check",
    &run_memory_watcher);

}
