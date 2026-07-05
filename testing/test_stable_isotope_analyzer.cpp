#include "test_runner.hpp"
#include "../modules/1534_stable_isotope_analyzer/stable_isotope_analyzer.hpp"

namespace {

testing::TestOutcome run_stable_isotope_analyzer() {
    stable_isotope_analyzer::init();
    auto s = stable_isotope_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stable_isotope_analyzer::shutdown();
    return testing::ok();
}

const int _reg_stable_isotope_analyzer = testing::register_test(
    "stable_isotope_analyzer",
    "1534_stable_isotope_analyzer: stub status check",
    &run_stable_isotope_analyzer);

}
