#include "test_runner.hpp"
#include "../modules/934_tolerance_stackup_analyzer/tolerance_stackup_analyzer.hpp"

namespace {

testing::TestOutcome run_tolerance_stackup_analyzer() {
    tolerance_stackup_analyzer::init();
    auto s = tolerance_stackup_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tolerance_stackup_analyzer::shutdown();
    return testing::ok();
}

const int _reg_tolerance_stackup_analyzer = testing::register_test(
    "tolerance_stackup_analyzer",
    "934_tolerance_stackup_analyzer: stub status check",
    &run_tolerance_stackup_analyzer);

}
