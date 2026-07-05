#include "test_runner.hpp"
#include "../modules/752_fallacy_detector/fallacy_detector.hpp"

namespace {

testing::TestOutcome run_fallacy_detector() {
    fallacy_detector::init();
    auto s = fallacy_detector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fallacy_detector::shutdown();
    return testing::ok();
}

const int _reg_fallacy_detector = testing::register_test(
    "fallacy_detector",
    "752_fallacy_detector: stub status check",
    &run_fallacy_detector);

}
