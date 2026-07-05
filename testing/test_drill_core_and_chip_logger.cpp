#include "test_runner.hpp"
#include "../modules/1098_drill_core_and_chip_logger/drill_core_and_chip_logger.hpp"

namespace {

testing::TestOutcome run_drill_core_and_chip_logger() {
    drill_core_and_chip_logger::init();
    auto s = drill_core_and_chip_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drill_core_and_chip_logger::shutdown();
    return testing::ok();
}

const int _reg_drill_core_and_chip_logger = testing::register_test(
    "drill_core_and_chip_logger",
    "1098_drill_core_and_chip_logger: stub status check",
    &run_drill_core_and_chip_logger);

}
