#include "test_runner.hpp"
#include "../modules/1653_screenshot_and_capture/screenshot_and_capture.hpp"

namespace {

testing::TestOutcome run_screenshot_and_capture() {
    screenshot_and_capture::init();
    auto s = screenshot_and_capture::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    screenshot_and_capture::shutdown();
    return testing::ok();
}

const int _reg_screenshot_and_capture = testing::register_test(
    "screenshot_and_capture",
    "1653_screenshot_and_capture: stub status check",
    &run_screenshot_and_capture);

}
