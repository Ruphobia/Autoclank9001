// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/356_memory_forensics_explorer/memory_forensics_explorer.hpp"

namespace {

testing::TestOutcome run_memory_forensics_explorer() {
    memory_forensics_explorer::init();
    auto s = memory_forensics_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    memory_forensics_explorer::shutdown();
    return testing::ok();
}

const int _reg_memory_forensics_explorer = testing::register_test(
    "memory_forensics_explorer",
    "356_memory_forensics_explorer: stub status check",
    &run_memory_forensics_explorer);

}
