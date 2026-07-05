#include "test_runner.hpp"
#include "../modules/1549_dive_log_decompression_planner/dive_log_decompression_planner.hpp"

namespace {

testing::TestOutcome run_dive_log_decompression_planner() {
    dive_log_decompression_planner::init();
    auto s = dive_log_decompression_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dive_log_decompression_planner::shutdown();
    return testing::ok();
}

const int _reg_dive_log_decompression_planner = testing::register_test(
    "dive_log_decompression_planner",
    "1549_dive_log_decompression_planner: stub status check",
    &run_dive_log_decompression_planner);

}
