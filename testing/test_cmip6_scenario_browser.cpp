#include "test_runner.hpp"
#include "../modules/1466_cmip6_scenario_browser/cmip6_scenario_browser.hpp"

namespace {

testing::TestOutcome run_cmip6_scenario_browser() {
    cmip6_scenario_browser::init();
    auto s = cmip6_scenario_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cmip6_scenario_browser::shutdown();
    return testing::ok();
}

const int _reg_cmip6_scenario_browser = testing::register_test(
    "cmip6_scenario_browser",
    "1466_cmip6_scenario_browser: stub status check",
    &run_cmip6_scenario_browser);

}
