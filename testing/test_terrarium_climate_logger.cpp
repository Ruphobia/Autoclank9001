#include "test_runner.hpp"
#include "../modules/2014_terrarium_climate_logger/terrarium_climate_logger.hpp"

namespace {

testing::TestOutcome run_terrarium_climate_logger() {
    terrarium_climate_logger::init();
    auto s = terrarium_climate_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    terrarium_climate_logger::shutdown();
    return testing::ok();
}

const int _reg_terrarium_climate_logger = testing::register_test(
    "terrarium_climate_logger",
    "2014_terrarium_climate_logger: stub status check",
    &run_terrarium_climate_logger);

}
