#include "test_runner.hpp"
#include "../modules/1340_lab_protocol_planner/lab_protocol_planner.hpp"

namespace {

testing::TestOutcome run_lab_protocol_planner() {
    lab_protocol_planner::init();
    auto s = lab_protocol_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lab_protocol_planner::shutdown();
    return testing::ok();
}

const int _reg_lab_protocol_planner = testing::register_test(
    "lab_protocol_planner",
    "1340_lab_protocol_planner: stub status check",
    &run_lab_protocol_planner);

}
