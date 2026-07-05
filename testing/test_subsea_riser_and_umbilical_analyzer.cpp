#include "test_runner.hpp"
#include "../modules/1116_subsea_riser_and_umbilical_analyzer/subsea_riser_and_umbilical_analyzer.hpp"

namespace {

testing::TestOutcome run_subsea_riser_and_umbilical_analyzer() {
    subsea_riser_and_umbilical_analyzer::init();
    auto s = subsea_riser_and_umbilical_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    subsea_riser_and_umbilical_analyzer::shutdown();
    return testing::ok();
}

const int _reg_subsea_riser_and_umbilical_analyzer = testing::register_test(
    "subsea_riser_and_umbilical_analyzer",
    "1116_subsea_riser_and_umbilical_analyzer: stub status check",
    &run_subsea_riser_and_umbilical_analyzer);

}
