#include "test_runner.hpp"
#include "../modules/1582_geocoder/geocoder.hpp"

namespace {

testing::TestOutcome run_geocoder() {
    geocoder::init();
    auto s = geocoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    geocoder::shutdown();
    return testing::ok();
}

const int _reg_geocoder = testing::register_test(
    "geocoder",
    "1582_geocoder: stub status check",
    &run_geocoder);

}
