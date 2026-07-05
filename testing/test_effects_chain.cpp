#include "test_runner.hpp"
#include "../modules/1731_effects_chain/effects_chain.hpp"

namespace {

testing::TestOutcome run_effects_chain() {
    effects_chain::init();
    auto s = effects_chain::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    effects_chain::shutdown();
    return testing::ok();
}

const int _reg_effects_chain = testing::register_test(
    "effects_chain",
    "1731_effects_chain: stub status check",
    &run_effects_chain);

}
