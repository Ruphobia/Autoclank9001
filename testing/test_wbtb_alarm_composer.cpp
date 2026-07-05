#include "test_runner.hpp"
#include "../modules/2158_wbtb_alarm_composer/wbtb_alarm_composer.hpp"

namespace {

testing::TestOutcome run_wbtb_alarm_composer() {
    wbtb_alarm_composer::init();
    auto s = wbtb_alarm_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wbtb_alarm_composer::shutdown();
    return testing::ok();
}

const int _reg_wbtb_alarm_composer = testing::register_test(
    "wbtb_alarm_composer",
    "2158_wbtb_alarm_composer: stub status check",
    &run_wbtb_alarm_composer);

}
