#include "test_runner.hpp"
#include "../modules/872_gaussian_beam_propagator/gaussian_beam_propagator.hpp"

namespace {

testing::TestOutcome run_gaussian_beam_propagator() {
    gaussian_beam_propagator::init();
    auto s = gaussian_beam_propagator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gaussian_beam_propagator::shutdown();
    return testing::ok();
}

const int _reg_gaussian_beam_propagator = testing::register_test(
    "gaussian_beam_propagator",
    "872_gaussian_beam_propagator: stub status check",
    &run_gaussian_beam_propagator);

}
