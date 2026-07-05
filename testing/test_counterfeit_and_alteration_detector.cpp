#include "test_runner.hpp"
#include "../modules/1890_counterfeit_and_alteration_detector/counterfeit_and_alteration_detector.hpp"

namespace {

testing::TestOutcome run_counterfeit_and_alteration_detector() {
    counterfeit_and_alteration_detector::init();
    auto s = counterfeit_and_alteration_detector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    counterfeit_and_alteration_detector::shutdown();
    return testing::ok();
}

const int _reg_counterfeit_and_alteration_detector = testing::register_test(
    "counterfeit_and_alteration_detector",
    "1890_counterfeit_and_alteration_detector: stub status check",
    &run_counterfeit_and_alteration_detector);

}
