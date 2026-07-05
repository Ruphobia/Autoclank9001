#include "test_runner.hpp"
#include "../modules/1200_can_bus_analyzer/can_bus_analyzer.hpp"

namespace {

testing::TestOutcome run_can_bus_analyzer() {
    can_bus_analyzer::init();
    auto s = can_bus_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    can_bus_analyzer::shutdown();
    return testing::ok();
}

const int _reg_can_bus_analyzer = testing::register_test(
    "can_bus_analyzer",
    "1200_can_bus_analyzer: stub status check",
    &run_can_bus_analyzer);

}
