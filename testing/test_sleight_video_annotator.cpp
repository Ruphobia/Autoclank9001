#include "test_runner.hpp"
#include "../modules/1931_sleight_video_annotator/sleight_video_annotator.hpp"

namespace {

testing::TestOutcome run_sleight_video_annotator() {
    sleight_video_annotator::init();
    auto s = sleight_video_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sleight_video_annotator::shutdown();
    return testing::ok();
}

const int _reg_sleight_video_annotator = testing::register_test(
    "sleight_video_annotator",
    "1931_sleight_video_annotator: stub status check",
    &run_sleight_video_annotator);

}
