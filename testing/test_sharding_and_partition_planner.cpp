// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/249_sharding_and_partition_planner/sharding_and_partition_planner.hpp"

namespace {

testing::TestOutcome run_sharding_and_partition_planner() {
    sharding_and_partition_planner::init();
    auto s = sharding_and_partition_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sharding_and_partition_planner::shutdown();
    return testing::ok();
}

const int _reg_sharding_and_partition_planner = testing::register_test(
    "sharding_and_partition_planner",
    "249_sharding_and_partition_planner: stub status check",
    &run_sharding_and_partition_planner);

}
