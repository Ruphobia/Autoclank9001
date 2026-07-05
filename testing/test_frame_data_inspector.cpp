#include "test_runner.hpp"
#include "../modules/1991_frame_data_inspector/frame_data_inspector.hpp"

namespace {

testing::TestOutcome run_frame_data_inspector() {
    frame_data_inspector::init();
    auto s = frame_data_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    frame_data_inspector::shutdown();
    return testing::ok();
}

const int _reg_frame_data_inspector = testing::register_test(
    "frame_data_inspector",
    "1991_frame_data_inspector: stub status check",
    &run_frame_data_inspector);

}
