#include "test_runner.hpp"
#include "../modules/989_heat_treatment_scheduler/heat_treatment_scheduler.hpp"

namespace {

testing::TestOutcome run_heat_treatment_scheduler() {
    heat_treatment_scheduler::init();
    auto s = heat_treatment_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    heat_treatment_scheduler::shutdown();
    return testing::ok();
}

const int _reg_heat_treatment_scheduler = testing::register_test(
    "heat_treatment_scheduler",
    "989_heat_treatment_scheduler: stub status check",
    &run_heat_treatment_scheduler);

}
