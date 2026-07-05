#include "test_runner.hpp"
#include "../modules/2108_colregs_and_lights_advisor/colregs_and_lights_advisor.hpp"

namespace {

testing::TestOutcome run_colregs_and_lights_advisor() {
    colregs_and_lights_advisor::init();
    auto s = colregs_and_lights_advisor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    colregs_and_lights_advisor::shutdown();
    return testing::ok();
}

const int _reg_colregs_and_lights_advisor = testing::register_test(
    "colregs_and_lights_advisor",
    "2108_colregs_and_lights_advisor: stub status check",
    &run_colregs_and_lights_advisor);

}
