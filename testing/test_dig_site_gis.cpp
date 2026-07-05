#include "test_runner.hpp"
#include "../modules/1524_dig_site_gis/dig_site_gis.hpp"

namespace {

testing::TestOutcome run_dig_site_gis() {
    dig_site_gis::init();
    auto s = dig_site_gis::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dig_site_gis::shutdown();
    return testing::ok();
}

const int _reg_dig_site_gis = testing::register_test(
    "dig_site_gis",
    "1524_dig_site_gis: stub status check",
    &run_dig_site_gis);

}
