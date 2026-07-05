#include "test_runner.hpp"
#include "../modules/317_stereo_view_inspector/stereo_view_inspector.hpp"

namespace {

testing::TestOutcome run_stereo_view_inspector() {
    stereo_view_inspector::init();
    auto s = stereo_view_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stereo_view_inspector::shutdown();
    return testing::ok();
}

const int _reg_stereo_view_inspector = testing::register_test(
    "stereo_view_inspector",
    "317_stereo_view_inspector: stub status check",
    &run_stereo_view_inspector);

}
