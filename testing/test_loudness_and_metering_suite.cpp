#include "test_runner.hpp"
#include "../modules/1734_loudness_and_metering_suite/loudness_and_metering_suite.hpp"

namespace {

testing::TestOutcome run_loudness_and_metering_suite() {
    loudness_and_metering_suite::init();
    auto s = loudness_and_metering_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    loudness_and_metering_suite::shutdown();
    return testing::ok();
}

const int _reg_loudness_and_metering_suite = testing::register_test(
    "loudness_and_metering_suite",
    "1734_loudness_and_metering_suite: stub status check",
    &run_loudness_and_metering_suite);

}
