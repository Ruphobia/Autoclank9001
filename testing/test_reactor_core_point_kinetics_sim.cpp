#include "test_runner.hpp"
#include "../modules/1102_reactor_core_point_kinetics_sim/reactor_core_point_kinetics_sim.hpp"

namespace {

testing::TestOutcome run_reactor_core_point_kinetics_sim() {
    reactor_core_point_kinetics_sim::init();
    auto s = reactor_core_point_kinetics_sim::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reactor_core_point_kinetics_sim::shutdown();
    return testing::ok();
}

const int _reg_reactor_core_point_kinetics_sim = testing::register_test(
    "reactor_core_point_kinetics_sim",
    "1102_reactor_core_point_kinetics_sim: stub status check",
    &run_reactor_core_point_kinetics_sim);

}
