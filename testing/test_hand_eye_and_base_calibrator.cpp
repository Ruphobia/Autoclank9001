#include "test_runner.hpp"
#include "../modules/1258_hand_eye_and_base_calibrator/hand_eye_and_base_calibrator.hpp"

namespace {

testing::TestOutcome run_hand_eye_and_base_calibrator() {
    hand_eye_and_base_calibrator::init();
    auto s = hand_eye_and_base_calibrator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hand_eye_and_base_calibrator::shutdown();
    return testing::ok();
}

const int _reg_hand_eye_and_base_calibrator = testing::register_test(
    "hand_eye_and_base_calibrator",
    "1258_hand_eye_and_base_calibrator: stub status check",
    &run_hand_eye_and_base_calibrator);

}
