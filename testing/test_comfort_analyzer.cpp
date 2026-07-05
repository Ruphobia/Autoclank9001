#include "test_runner.hpp"
#include "../modules/314_comfort_analyzer/comfort_analyzer.hpp"

namespace {

testing::TestOutcome run_comfort_analyzer() {
    comfort_analyzer::init();
    auto s = comfort_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    comfort_analyzer::shutdown();
    return testing::ok();
}

const int _reg_comfort_analyzer = testing::register_test(
    "comfort_analyzer",
    "314_comfort_analyzer: stub status check",
    &run_comfort_analyzer);

}
