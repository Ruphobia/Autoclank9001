#include "test_runner.hpp"
#include "../modules/1034_frame_analyzer/frame_analyzer.hpp"

namespace {

testing::TestOutcome run_frame_analyzer() {
    frame_analyzer::init();
    auto s = frame_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    frame_analyzer::shutdown();
    return testing::ok();
}

const int _reg_frame_analyzer = testing::register_test(
    "frame_analyzer",
    "1034_frame_analyzer: stub status check",
    &run_frame_analyzer);

}
