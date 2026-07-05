#include "test_runner.hpp"
#include "../modules/454_stakeholder_and_raci_map/stakeholder_and_raci_map.hpp"

namespace {

testing::TestOutcome run_stakeholder_and_raci_map() {
    stakeholder_and_raci_map::init();
    auto s = stakeholder_and_raci_map::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stakeholder_and_raci_map::shutdown();
    return testing::ok();
}

const int _reg_stakeholder_and_raci_map = testing::register_test(
    "stakeholder_and_raci_map",
    "454_stakeholder_and_raci_map: stub status check",
    &run_stakeholder_and_raci_map);

}
