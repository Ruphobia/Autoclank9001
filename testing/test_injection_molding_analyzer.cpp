#include "test_runner.hpp"
#include "../modules/943_injection_molding_analyzer/injection_molding_analyzer.hpp"

namespace {

testing::TestOutcome run_injection_molding_analyzer() {
    injection_molding_analyzer::init();
    auto s = injection_molding_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    injection_molding_analyzer::shutdown();
    return testing::ok();
}

const int _reg_injection_molding_analyzer = testing::register_test(
    "injection_molding_analyzer",
    "943_injection_molding_analyzer: stub status check",
    &run_injection_molding_analyzer);

}
