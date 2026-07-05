#include "test_runner.hpp"
#include "../modules/115_component_scaffolder/component_scaffolder.hpp"

namespace {

testing::TestOutcome run_component_scaffolder() {
    component_scaffolder::init();
    auto s = component_scaffolder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    component_scaffolder::shutdown();
    return testing::ok();
}

const int _reg_component_scaffolder = testing::register_test(
    "component_scaffolder",
    "115_component_scaffolder: stub status check",
    &run_component_scaffolder);

}
