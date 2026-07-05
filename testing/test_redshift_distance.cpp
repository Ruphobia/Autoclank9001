#include "test_runner.hpp"
#include "../modules/1486_redshift_distance/redshift_distance.hpp"

namespace {

testing::TestOutcome run_redshift_distance() {
    redshift_distance::init();
    auto s = redshift_distance::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    redshift_distance::shutdown();
    return testing::ok();
}

const int _reg_redshift_distance = testing::register_test(
    "redshift_distance",
    "1486_redshift_distance: stub status check",
    &run_redshift_distance);

}
