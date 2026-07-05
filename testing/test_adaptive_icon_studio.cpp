#include "test_runner.hpp"
#include "../modules/161_adaptive_icon_studio/adaptive_icon_studio.hpp"

namespace {

testing::TestOutcome run_adaptive_icon_studio() {
    adaptive_icon_studio::init();
    auto s = adaptive_icon_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    adaptive_icon_studio::shutdown();
    return testing::ok();
}

const int _reg_adaptive_icon_studio = testing::register_test(
    "adaptive_icon_studio",
    "161_adaptive_icon_studio: stub status check",
    &run_adaptive_icon_studio);

}
