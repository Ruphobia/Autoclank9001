#include "test_runner.hpp"
#include "../modules/661_survival_analysis/survival_analysis.hpp"

namespace {

testing::TestOutcome run_survival_analysis() {
    survival_analysis::init();
    auto s = survival_analysis::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    survival_analysis::shutdown();
    return testing::ok();
}

const int _reg_survival_analysis = testing::register_test(
    "survival_analysis",
    "661_survival_analysis: stub status check",
    &run_survival_analysis);

}
