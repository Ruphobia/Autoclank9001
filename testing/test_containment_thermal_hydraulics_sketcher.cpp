#include "test_runner.hpp"
#include "../modules/1109_containment_thermal_hydraulics_sketcher/containment_thermal_hydraulics_sketcher.hpp"

namespace {

testing::TestOutcome run_containment_thermal_hydraulics_sketcher() {
    containment_thermal_hydraulics_sketcher::init();
    auto s = containment_thermal_hydraulics_sketcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    containment_thermal_hydraulics_sketcher::shutdown();
    return testing::ok();
}

const int _reg_containment_thermal_hydraulics_sketcher = testing::register_test(
    "containment_thermal_hydraulics_sketcher",
    "1109_containment_thermal_hydraulics_sketcher: stub status check",
    &run_containment_thermal_hydraulics_sketcher);

}
