#include "test_runner.hpp"
#include "../modules/1533_sediment_and_soil_logger/sediment_and_soil_logger.hpp"

namespace {

testing::TestOutcome run_sediment_and_soil_logger() {
    sediment_and_soil_logger::init();
    auto s = sediment_and_soil_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sediment_and_soil_logger::shutdown();
    return testing::ok();
}

const int _reg_sediment_and_soil_logger = testing::register_test(
    "sediment_and_soil_logger",
    "1533_sediment_and_soil_logger: stub status check",
    &run_sediment_and_soil_logger);

}
