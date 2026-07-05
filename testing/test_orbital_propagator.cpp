#include "test_runner.hpp"
#include "../modules/1127_orbital_propagator/orbital_propagator.hpp"

namespace {

testing::TestOutcome run_orbital_propagator() {
    orbital_propagator::init();
    auto s = orbital_propagator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    orbital_propagator::shutdown();
    return testing::ok();
}

const int _reg_orbital_propagator = testing::register_test(
    "orbital_propagator",
    "1127_orbital_propagator: stub status check",
    &run_orbital_propagator);

}
