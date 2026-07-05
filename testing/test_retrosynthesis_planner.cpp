#include "test_runner.hpp"
#include "../modules/1300_retrosynthesis_planner/retrosynthesis_planner.hpp"

namespace {

testing::TestOutcome run_retrosynthesis_planner() {
    retrosynthesis_planner::init();
    auto s = retrosynthesis_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    retrosynthesis_planner::shutdown();
    return testing::ok();
}

const int _reg_retrosynthesis_planner = testing::register_test(
    "retrosynthesis_planner",
    "1300_retrosynthesis_planner: stub status check",
    &run_retrosynthesis_planner);

}
