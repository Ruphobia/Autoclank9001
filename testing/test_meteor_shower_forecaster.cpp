#include "test_runner.hpp"
#include "../modules/1496_meteor_shower_forecaster/meteor_shower_forecaster.hpp"

namespace {

testing::TestOutcome run_meteor_shower_forecaster() {
    meteor_shower_forecaster::init();
    auto s = meteor_shower_forecaster::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    meteor_shower_forecaster::shutdown();
    return testing::ok();
}

const int _reg_meteor_shower_forecaster = testing::register_test(
    "meteor_shower_forecaster",
    "1496_meteor_shower_forecaster: stub status check",
    &run_meteor_shower_forecaster);

}
