#include "test_runner.hpp"
#include "../modules/915_control_systems_designer/control_systems_designer.hpp"

namespace {

testing::TestOutcome run_control_systems_designer() {
    control_systems_designer::init();
    auto s = control_systems_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    control_systems_designer::shutdown();
    return testing::ok();
}

const int _reg_control_systems_designer = testing::register_test(
    "control_systems_designer",
    "915_control_systems_designer: stub status check",
    &run_control_systems_designer);

}
