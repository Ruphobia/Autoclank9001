#include "test_runner.hpp"
#include "../modules/1092_pit_optimizer/pit_optimizer.hpp"

namespace {

testing::TestOutcome run_pit_optimizer() {
    pit_optimizer::init();
    auto s = pit_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pit_optimizer::shutdown();
    return testing::ok();
}

const int _reg_pit_optimizer = testing::register_test(
    "pit_optimizer",
    "1092_pit_optimizer: stub status check",
    &run_pit_optimizer);

}
