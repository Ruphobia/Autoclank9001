#include "test_runner.hpp"
#include "../modules/1514_incubation_and_reproduction_tracker/incubation_and_reproduction_tracker.hpp"

namespace {

testing::TestOutcome run_incubation_and_reproduction_tracker() {
    incubation_and_reproduction_tracker::init();
    auto s = incubation_and_reproduction_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    incubation_and_reproduction_tracker::shutdown();
    return testing::ok();
}

const int _reg_incubation_and_reproduction_tracker = testing::register_test(
    "incubation_and_reproduction_tracker",
    "1514_incubation_and_reproduction_tracker: stub status check",
    &run_incubation_and_reproduction_tracker);

}
