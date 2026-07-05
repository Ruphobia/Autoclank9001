#include "test_runner.hpp"
#include "../modules/2099_nmea_0183_2000_multiplexer/nmea_0183_2000_multiplexer.hpp"

namespace {

testing::TestOutcome run_nmea_0183_2000_multiplexer() {
    nmea_0183_2000_multiplexer::init();
    auto s = nmea_0183_2000_multiplexer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nmea_0183_2000_multiplexer::shutdown();
    return testing::ok();
}

const int _reg_nmea_0183_2000_multiplexer = testing::register_test(
    "nmea_0183_2000_multiplexer",
    "2099_nmea_0183_2000_multiplexer: stub status check",
    &run_nmea_0183_2000_multiplexer);

}
