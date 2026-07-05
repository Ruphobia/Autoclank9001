#include "test_runner.hpp"
#include "../modules/1166_aero_optimizer/aero_optimizer.hpp"

namespace {

testing::TestOutcome run_aero_optimizer() {
    aero_optimizer::init();
    auto s = aero_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    aero_optimizer::shutdown();
    return testing::ok();
}

const int _reg_aero_optimizer = testing::register_test(
    "aero_optimizer",
    "1166_aero_optimizer: stub status check",
    &run_aero_optimizer);

}
