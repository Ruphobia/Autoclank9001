#include "test_runner.hpp"
#include "../modules/1241_tire_wear_alignment_tracker/tire_wear_alignment_tracker.hpp"

namespace {

testing::TestOutcome run_tire_wear_alignment_tracker() {
    tire_wear_alignment_tracker::init();
    auto s = tire_wear_alignment_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tire_wear_alignment_tracker::shutdown();
    return testing::ok();
}

const int _reg_tire_wear_alignment_tracker = testing::register_test(
    "tire_wear_alignment_tracker",
    "1241_tire_wear_alignment_tracker: stub status check",
    &run_tire_wear_alignment_tracker);

}
