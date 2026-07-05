#include "test_runner.hpp"
#include "../modules/1933_stack_and_setup_manager/stack_and_setup_manager.hpp"

namespace {

testing::TestOutcome run_stack_and_setup_manager() {
    stack_and_setup_manager::init();
    auto s = stack_and_setup_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stack_and_setup_manager::shutdown();
    return testing::ok();
}

const int _reg_stack_and_setup_manager = testing::register_test(
    "stack_and_setup_manager",
    "1933_stack_and_setup_manager: stub status check",
    &run_stack_and_setup_manager);

}
