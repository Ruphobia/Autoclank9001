#include "test_runner.hpp"
#include "../modules/2126_hf_propagation_forecaster/hf_propagation_forecaster.hpp"

namespace {

testing::TestOutcome run_hf_propagation_forecaster() {
    hf_propagation_forecaster::init();
    auto s = hf_propagation_forecaster::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hf_propagation_forecaster::shutdown();
    return testing::ok();
}

const int _reg_hf_propagation_forecaster = testing::register_test(
    "hf_propagation_forecaster",
    "2126_hf_propagation_forecaster: stub status check",
    &run_hf_propagation_forecaster);

}
