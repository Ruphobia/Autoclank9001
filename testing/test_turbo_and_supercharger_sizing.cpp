#include "test_runner.hpp"
#include "../modules/1206_turbo_and_supercharger_sizing/turbo_and_supercharger_sizing.hpp"

namespace {

testing::TestOutcome run_turbo_and_supercharger_sizing() {
    turbo_and_supercharger_sizing::init();
    auto s = turbo_and_supercharger_sizing::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    turbo_and_supercharger_sizing::shutdown();
    return testing::ok();
}

const int _reg_turbo_and_supercharger_sizing = testing::register_test(
    "turbo_and_supercharger_sizing",
    "1206_turbo_and_supercharger_sizing: stub status check",
    &run_turbo_and_supercharger_sizing);

}
