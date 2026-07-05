#include "test_runner.hpp"
#include "../modules/1292_thermal_performance_modeler/thermal_performance_modeler.hpp"

namespace {

testing::TestOutcome run_thermal_performance_modeler() {
    thermal_performance_modeler::init();
    auto s = thermal_performance_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    thermal_performance_modeler::shutdown();
    return testing::ok();
}

const int _reg_thermal_performance_modeler = testing::register_test(
    "thermal_performance_modeler",
    "1292_thermal_performance_modeler: stub status check",
    &run_thermal_performance_modeler);

}
