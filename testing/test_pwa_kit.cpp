#include "test_runner.hpp"
#include "../modules/129_pwa_kit/pwa_kit.hpp"

namespace {

testing::TestOutcome run_pwa_kit() {
    pwa_kit::init();
    auto s = pwa_kit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pwa_kit::shutdown();
    return testing::ok();
}

const int _reg_pwa_kit = testing::register_test(
    "pwa_kit",
    "129_pwa_kit: stub status check",
    &run_pwa_kit);

}
