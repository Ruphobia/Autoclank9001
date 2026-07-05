#include "test_runner.hpp"
#include "../modules/847_component_library_manager/component_library_manager.hpp"

namespace {

testing::TestOutcome run_component_library_manager() {
    component_library_manager::init();
    auto s = component_library_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    component_library_manager::shutdown();
    return testing::ok();
}

const int _reg_component_library_manager = testing::register_test(
    "component_library_manager",
    "847_component_library_manager: stub status check",
    &run_component_library_manager);

}
