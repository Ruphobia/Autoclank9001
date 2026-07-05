#include "test_runner.hpp"
#include "../modules/684_timeline_composer/timeline_composer.hpp"

namespace {

testing::TestOutcome run_timeline_composer() {
    timeline_composer::init();
    auto s = timeline_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    timeline_composer::shutdown();
    return testing::ok();
}

const int _reg_timeline_composer = testing::register_test(
    "timeline_composer",
    "684_timeline_composer: stub status check",
    &run_timeline_composer);

}
