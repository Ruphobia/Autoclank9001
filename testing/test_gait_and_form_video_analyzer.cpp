#include "test_runner.hpp"
#include "../modules/2076_gait_and_form_video_analyzer/gait_and_form_video_analyzer.hpp"

namespace {

testing::TestOutcome run_gait_and_form_video_analyzer() {
    gait_and_form_video_analyzer::init();
    auto s = gait_and_form_video_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gait_and_form_video_analyzer::shutdown();
    return testing::ok();
}

const int _reg_gait_and_form_video_analyzer = testing::register_test(
    "gait_and_form_video_analyzer",
    "2076_gait_and_form_video_analyzer: stub status check",
    &run_gait_and_form_video_analyzer);

}
