#include "test_runner.hpp"
#include "../modules/1641_frame_interpolator/frame_interpolator.hpp"

namespace {

testing::TestOutcome run_frame_interpolator() {
    frame_interpolator::init();
    auto s = frame_interpolator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    frame_interpolator::shutdown();
    return testing::ok();
}

const int _reg_frame_interpolator = testing::register_test(
    "frame_interpolator",
    "1641_frame_interpolator: stub status check",
    &run_frame_interpolator);

}
