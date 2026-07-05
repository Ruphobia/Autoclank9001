#include "test_runner.hpp"
#include "../modules/2161_light_exposure_logger/light_exposure_logger.hpp"

namespace {

testing::TestOutcome run_light_exposure_logger() {
    light_exposure_logger::init();
    auto s = light_exposure_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    light_exposure_logger::shutdown();
    return testing::ok();
}

const int _reg_light_exposure_logger = testing::register_test(
    "light_exposure_logger",
    "2161_light_exposure_logger: stub status check",
    &run_light_exposure_logger);

}
