#include "test_runner.hpp"
#include "../modules/1107_alara_dose_planner/alara_dose_planner.hpp"

namespace {

testing::TestOutcome run_alara_dose_planner() {
    alara_dose_planner::init();
    auto s = alara_dose_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    alara_dose_planner::shutdown();
    return testing::ok();
}

const int _reg_alara_dose_planner = testing::register_test(
    "alara_dose_planner",
    "1107_alara_dose_planner: stub status check",
    &run_alara_dose_planner);

}
