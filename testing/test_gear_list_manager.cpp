#include "test_runner.hpp"
#include "../modules/2063_gear_list_manager/gear_list_manager.hpp"

namespace {

testing::TestOutcome run_gear_list_manager() {
    gear_list_manager::init();
    auto s = gear_list_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gear_list_manager::shutdown();
    return testing::ok();
}

const int _reg_gear_list_manager = testing::register_test(
    "gear_list_manager",
    "2063_gear_list_manager: stub status check",
    &run_gear_list_manager);

}
