#include "test_runner.hpp"
#include "../modules/1271_solar_resource_fetcher/solar_resource_fetcher.hpp"

namespace {

testing::TestOutcome run_solar_resource_fetcher() {
    solar_resource_fetcher::init();
    auto s = solar_resource_fetcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    solar_resource_fetcher::shutdown();
    return testing::ok();
}

const int _reg_solar_resource_fetcher = testing::register_test(
    "solar_resource_fetcher",
    "1271_solar_resource_fetcher: stub status check",
    &run_solar_resource_fetcher);

}
