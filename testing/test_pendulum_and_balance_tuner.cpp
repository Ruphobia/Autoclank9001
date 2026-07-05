#include "test_runner.hpp"
#include "../modules/1839_pendulum_and_balance_tuner/pendulum_and_balance_tuner.hpp"

namespace {

testing::TestOutcome run_pendulum_and_balance_tuner() {
    pendulum_and_balance_tuner::init();
    auto s = pendulum_and_balance_tuner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pendulum_and_balance_tuner::shutdown();
    return testing::ok();
}

const int _reg_pendulum_and_balance_tuner = testing::register_test(
    "pendulum_and_balance_tuner",
    "1839_pendulum_and_balance_tuner: stub status check",
    &run_pendulum_and_balance_tuner);

}
