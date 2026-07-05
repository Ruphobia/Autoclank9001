#include "test_runner.hpp"
#include "../modules/867_reliability_and_fmea/reliability_and_fmea.hpp"

namespace {

testing::TestOutcome run_reliability_and_fmea() {
    reliability_and_fmea::init();
    auto s = reliability_and_fmea::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reliability_and_fmea::shutdown();
    return testing::ok();
}

const int _reg_reliability_and_fmea = testing::register_test(
    "reliability_and_fmea",
    "867_reliability_and_fmea: stub status check",
    &run_reliability_and_fmea);

}
