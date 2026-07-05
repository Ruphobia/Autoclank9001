#include "test_runner.hpp"
#include "../modules/1442_precinct_to_district_joiner/precinct_to_district_joiner.hpp"

namespace {

testing::TestOutcome run_precinct_to_district_joiner() {
    precinct_to_district_joiner::init();
    auto s = precinct_to_district_joiner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    precinct_to_district_joiner::shutdown();
    return testing::ok();
}

const int _reg_precinct_to_district_joiner = testing::register_test(
    "precinct_to_district_joiner",
    "1442_precinct_to_district_joiner: stub status check",
    &run_precinct_to_district_joiner);

}
