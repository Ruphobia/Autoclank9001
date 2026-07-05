#include "test_runner.hpp"
#include "../modules/1312_property_predictor/property_predictor.hpp"

namespace {

testing::TestOutcome run_property_predictor() {
    property_predictor::init();
    auto s = property_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    property_predictor::shutdown();
    return testing::ok();
}

const int _reg_property_predictor = testing::register_test(
    "property_predictor",
    "1312_property_predictor: stub status check",
    &run_property_predictor);

}
