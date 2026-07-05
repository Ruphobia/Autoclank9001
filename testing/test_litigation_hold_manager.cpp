#include "test_runner.hpp"
#include "../modules/597_litigation_hold_manager/litigation_hold_manager.hpp"

namespace {

testing::TestOutcome run_litigation_hold_manager() {
    litigation_hold_manager::init();
    auto s = litigation_hold_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    litigation_hold_manager::shutdown();
    return testing::ok();
}

const int _reg_litigation_hold_manager = testing::register_test(
    "litigation_hold_manager",
    "597_litigation_hold_manager: stub status check",
    &run_litigation_hold_manager);

}
