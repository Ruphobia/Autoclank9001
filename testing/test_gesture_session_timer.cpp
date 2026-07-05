#include "test_runner.hpp"
#include "../modules/1873_gesture_session_timer/gesture_session_timer.hpp"

namespace {

testing::TestOutcome run_gesture_session_timer() {
    gesture_session_timer::init();
    auto s = gesture_session_timer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gesture_session_timer::shutdown();
    return testing::ok();
}

const int _reg_gesture_session_timer = testing::register_test(
    "gesture_session_timer",
    "1873_gesture_session_timer: stub status check",
    &run_gesture_session_timer);

}
