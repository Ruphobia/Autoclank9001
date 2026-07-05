#include "test_runner.hpp"
#include "../modules/761_ethics_scenario_explorer/ethics_scenario_explorer.hpp"

namespace {

testing::TestOutcome run_ethics_scenario_explorer() {
    ethics_scenario_explorer::init();
    auto s = ethics_scenario_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ethics_scenario_explorer::shutdown();
    return testing::ok();
}

const int _reg_ethics_scenario_explorer = testing::register_test(
    "ethics_scenario_explorer",
    "761_ethics_scenario_explorer: stub status check",
    &run_ethics_scenario_explorer);

}
