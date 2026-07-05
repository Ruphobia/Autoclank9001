#include "test_runner.hpp"
#include "../modules/473_weibull_reliability_analyzer/weibull_reliability_analyzer.hpp"

namespace {

testing::TestOutcome run_weibull_reliability_analyzer() {
    weibull_reliability_analyzer::init();
    auto s = weibull_reliability_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    weibull_reliability_analyzer::shutdown();
    return testing::ok();
}

const int _reg_weibull_reliability_analyzer = testing::register_test(
    "weibull_reliability_analyzer",
    "473_weibull_reliability_analyzer: stub status check",
    &run_weibull_reliability_analyzer);

}
