#include "test_runner.hpp"
#include "../modules/1949_initiative_tracker/initiative_tracker.hpp"

namespace {

testing::TestOutcome run_initiative_tracker() {
    initiative_tracker::init();
    auto s = initiative_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    initiative_tracker::shutdown();
    return testing::ok();
}

const int _reg_initiative_tracker = testing::register_test(
    "initiative_tracker",
    "1949_initiative_tracker: stub status check",
    &run_initiative_tracker);

}
