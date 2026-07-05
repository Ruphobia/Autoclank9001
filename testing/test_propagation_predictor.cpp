#include "test_runner.hpp"
#include "../modules/1187_propagation_predictor/propagation_predictor.hpp"

namespace {

testing::TestOutcome run_propagation_predictor() {
    propagation_predictor::init();
    auto s = propagation_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    propagation_predictor::shutdown();
    return testing::ok();
}

const int _reg_propagation_predictor = testing::register_test(
    "propagation_predictor",
    "1187_propagation_predictor: stub status check",
    &run_propagation_predictor);

}
