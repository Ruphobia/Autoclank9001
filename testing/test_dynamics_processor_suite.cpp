#include "test_runner.hpp"
#include "../modules/1733_dynamics_processor_suite/dynamics_processor_suite.hpp"

namespace {

testing::TestOutcome run_dynamics_processor_suite() {
    dynamics_processor_suite::init();
    auto s = dynamics_processor_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dynamics_processor_suite::shutdown();
    return testing::ok();
}

const int _reg_dynamics_processor_suite = testing::register_test(
    "dynamics_processor_suite",
    "1733_dynamics_processor_suite: stub status check",
    &run_dynamics_processor_suite);

}
