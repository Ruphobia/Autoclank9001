#include "test_runner.hpp"
#include "../modules/722_scribal_hand_classifier/scribal_hand_classifier.hpp"

namespace {

testing::TestOutcome run_scribal_hand_classifier() {
    scribal_hand_classifier::init();
    auto s = scribal_hand_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scribal_hand_classifier::shutdown();
    return testing::ok();
}

const int _reg_scribal_hand_classifier = testing::register_test(
    "scribal_hand_classifier",
    "722_scribal_hand_classifier: stub status check",
    &run_scribal_hand_classifier);

}
