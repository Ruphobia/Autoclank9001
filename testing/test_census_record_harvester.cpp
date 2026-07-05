#include "test_runner.hpp"
#include "../modules/2137_census_record_harvester/census_record_harvester.hpp"

namespace {

testing::TestOutcome run_census_record_harvester() {
    census_record_harvester::init();
    auto s = census_record_harvester::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    census_record_harvester::shutdown();
    return testing::ok();
}

const int _reg_census_record_harvester = testing::register_test(
    "census_record_harvester",
    "2137_census_record_harvester: stub status check",
    &run_census_record_harvester);

}
