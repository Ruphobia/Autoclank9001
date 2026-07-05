#include "test_runner.hpp"
#include "../modules/1491_satellite_tracker/satellite_tracker.hpp"

namespace {

testing::TestOutcome run_satellite_tracker() {
    satellite_tracker::init();
    auto s = satellite_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    satellite_tracker::shutdown();
    return testing::ok();
}

const int _reg_satellite_tracker = testing::register_test(
    "satellite_tracker",
    "1491_satellite_tracker: stub status check",
    &run_satellite_tracker);

}
