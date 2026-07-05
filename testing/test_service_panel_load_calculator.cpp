#include "test_runner.hpp"
#include "../modules/2019_service_panel_load_calculator/service_panel_load_calculator.hpp"

namespace {

testing::TestOutcome run_service_panel_load_calculator() {
    service_panel_load_calculator::init();
    auto s = service_panel_load_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    service_panel_load_calculator::shutdown();
    return testing::ok();
}

const int _reg_service_panel_load_calculator = testing::register_test(
    "service_panel_load_calculator",
    "2019_service_panel_load_calculator: stub status check",
    &run_service_panel_load_calculator);

}
