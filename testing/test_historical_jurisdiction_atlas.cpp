#include "test_runner.hpp"
#include "../modules/2142_historical_jurisdiction_atlas/historical_jurisdiction_atlas.hpp"

namespace {

testing::TestOutcome run_historical_jurisdiction_atlas() {
    historical_jurisdiction_atlas::init();
    auto s = historical_jurisdiction_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    historical_jurisdiction_atlas::shutdown();
    return testing::ok();
}

const int _reg_historical_jurisdiction_atlas = testing::register_test(
    "historical_jurisdiction_atlas",
    "2142_historical_jurisdiction_atlas: stub status check",
    &run_historical_jurisdiction_atlas);

}
