#include "test_runner.hpp"
#include "../modules/1083_pvt_and_equation_of_state_analyzer/pvt_and_equation_of_state_analyzer.hpp"

namespace {

testing::TestOutcome run_pvt_and_equation_of_state_analyzer() {
    pvt_and_equation_of_state_analyzer::init();
    auto s = pvt_and_equation_of_state_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pvt_and_equation_of_state_analyzer::shutdown();
    return testing::ok();
}

const int _reg_pvt_and_equation_of_state_analyzer = testing::register_test(
    "pvt_and_equation_of_state_analyzer",
    "1083_pvt_and_equation_of_state_analyzer: stub status check",
    &run_pvt_and_equation_of_state_analyzer);

}
