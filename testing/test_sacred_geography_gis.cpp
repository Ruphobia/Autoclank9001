#include "test_runner.hpp"
#include "../modules/765_sacred_geography_gis/sacred_geography_gis.hpp"

namespace {

testing::TestOutcome run_sacred_geography_gis() {
    sacred_geography_gis::init();
    auto s = sacred_geography_gis::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sacred_geography_gis::shutdown();
    return testing::ok();
}

const int _reg_sacred_geography_gis = testing::register_test(
    "sacred_geography_gis",
    "765_sacred_geography_gis: stub status check",
    &run_sacred_geography_gis);

}
