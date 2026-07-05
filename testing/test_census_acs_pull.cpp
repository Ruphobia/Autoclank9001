#include "test_runner.hpp"
#include "../modules/1437_census_acs_pull/census_acs_pull.hpp"

namespace {

testing::TestOutcome run_census_acs_pull() {
    census_acs_pull::init();
    auto s = census_acs_pull::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    census_acs_pull::shutdown();
    return testing::ok();
}

const int _reg_census_acs_pull = testing::register_test(
    "census_acs_pull",
    "1437_census_acs_pull: stub status check",
    &run_census_acs_pull);

}
