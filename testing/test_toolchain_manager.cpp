#include "test_runner.hpp"
#include "../modules/181_toolchain_manager/toolchain_manager.hpp"

namespace {

testing::TestOutcome run_toolchain_manager() {
    toolchain_manager::init();
    auto s = toolchain_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    toolchain_manager::shutdown();
    return testing::ok();
}

const int _reg_toolchain_manager = testing::register_test(
    "toolchain_manager",
    "181_toolchain_manager: stub status check",
    &run_toolchain_manager);

}
