#include "test_runner.hpp"
#include "../modules/1639_video_editor/video_editor.hpp"

namespace {

testing::TestOutcome run_video_editor() {
    video_editor::init();
    auto s = video_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    video_editor::shutdown();
    return testing::ok();
}

const int _reg_video_editor = testing::register_test(
    "video_editor",
    "1639_video_editor: stub status check",
    &run_video_editor);

}
