#include "test_runner.hpp"
#include "../modules/947_vibration_analyzer/vibration_analyzer.hpp"

namespace {

testing::TestOutcome run_vibration_analyzer() {
    vibration_analyzer::init();
    auto s = vibration_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vibration_analyzer::shutdown();
    return testing::ok();
}

const int _reg_vibration_analyzer = testing::register_test(
    "vibration_analyzer",
    "947_vibration_analyzer: stub status check",
    &run_vibration_analyzer);

}
