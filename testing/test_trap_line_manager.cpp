#include "test_runner.hpp"
#include "../modules/2056_trap_line_manager/trap_line_manager.hpp"

namespace {

testing::TestOutcome run_trap_line_manager() {
    trap_line_manager::init();
    auto s = trap_line_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    trap_line_manager::shutdown();
    return testing::ok();
}

const int _reg_trap_line_manager = testing::register_test(
    "trap_line_manager",
    "2056_trap_line_manager: stub status check",
    &run_trap_line_manager);

}
