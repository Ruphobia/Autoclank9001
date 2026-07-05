#include "test_runner.hpp"
#include "../modules/401_capacitor_atlas/capacitor_atlas.hpp"

namespace {

testing::TestOutcome run_capacitor_atlas() {
    capacitor_atlas::init();
    auto s = capacitor_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    capacitor_atlas::shutdown();
    return testing::ok();
}

const int _reg_capacitor_atlas = testing::register_test(
    "capacitor_atlas",
    "401_capacitor_atlas: stub status check",
    &run_capacitor_atlas);

}
