#include "test_runner.hpp"
#include "../modules/1671_storyboard_composer/storyboard_composer.hpp"

namespace {

testing::TestOutcome run_storyboard_composer() {
    storyboard_composer::init();
    auto s = storyboard_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    storyboard_composer::shutdown();
    return testing::ok();
}

const int _reg_storyboard_composer = testing::register_test(
    "storyboard_composer",
    "1671_storyboard_composer: stub status check",
    &run_storyboard_composer);

}
