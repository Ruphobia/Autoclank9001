#include "test_runner.hpp"
#include "../modules/1754_granular_and_concatenative_synthesis_lab/granular_and_concatenative_synthesis_lab.hpp"

namespace {

testing::TestOutcome run_granular_and_concatenative_synthesis_lab() {
    granular_and_concatenative_synthesis_lab::init();
    auto s = granular_and_concatenative_synthesis_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    granular_and_concatenative_synthesis_lab::shutdown();
    return testing::ok();
}

const int _reg_granular_and_concatenative_synthesis_lab = testing::register_test(
    "granular_and_concatenative_synthesis_lab",
    "1754_granular_and_concatenative_synthesis_lab: stub status check",
    &run_granular_and_concatenative_synthesis_lab);

}
