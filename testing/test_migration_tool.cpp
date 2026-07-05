// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/227_migration_tool/migration_tool.hpp"

namespace {

testing::TestOutcome run_migration_tool() {
    migration_tool::init();
    auto s = migration_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    migration_tool::shutdown();
    return testing::ok();
}

const int _reg_migration_tool = testing::register_test(
    "migration_tool",
    "227_migration_tool: stub status check",
    &run_migration_tool);

}
