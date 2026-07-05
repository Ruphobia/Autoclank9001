#include "test_runner.hpp"
#include "../modules/1381_outbreak_and_transmission_tracker/outbreak_and_transmission_tracker.hpp"

namespace {

testing::TestOutcome run_outbreak_and_transmission_tracker() {
    outbreak_and_transmission_tracker::init();
    auto s = outbreak_and_transmission_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    outbreak_and_transmission_tracker::shutdown();
    return testing::ok();
}

const int _reg_outbreak_and_transmission_tracker = testing::register_test(
    "outbreak_and_transmission_tracker",
    "1381_outbreak_and_transmission_tracker: stub status check",
    &run_outbreak_and_transmission_tracker);

}
