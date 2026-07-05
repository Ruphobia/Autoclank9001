#include "test_runner.hpp"
#include "../modules/1176_ads_b_traffic_console/ads_b_traffic_console.hpp"

namespace {

testing::TestOutcome run_ads_b_traffic_console() {
    ads_b_traffic_console::init();
    auto s = ads_b_traffic_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ads_b_traffic_console::shutdown();
    return testing::ok();
}

const int _reg_ads_b_traffic_console = testing::register_test(
    "ads_b_traffic_console",
    "1176_ads_b_traffic_console: stub status check",
    &run_ads_b_traffic_console);

}
