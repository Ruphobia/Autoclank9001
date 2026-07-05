#include "test_runner.hpp"
#include "../modules/1944_reveal_detector/reveal_detector.hpp"

namespace {

testing::TestOutcome run_reveal_detector() {
    reveal_detector::init();
    auto s = reveal_detector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reveal_detector::shutdown();
    return testing::ok();
}

const int _reg_reveal_detector = testing::register_test(
    "reveal_detector",
    "1944_reveal_detector: stub status check",
    &run_reveal_detector);

}
