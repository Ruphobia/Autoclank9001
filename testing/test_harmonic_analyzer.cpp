#include "test_runner.hpp"
#include "../modules/1705_harmonic_analyzer/harmonic_analyzer.hpp"

namespace {

testing::TestOutcome run_harmonic_analyzer() {
    harmonic_analyzer::init();
    auto s = harmonic_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    harmonic_analyzer::shutdown();
    return testing::ok();
}

const int _reg_harmonic_analyzer = testing::register_test(
    "harmonic_analyzer",
    "1705_harmonic_analyzer: stub status check",
    &run_harmonic_analyzer);

}
