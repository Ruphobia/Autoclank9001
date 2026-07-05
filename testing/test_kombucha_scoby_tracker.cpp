#include "test_runner.hpp"
#include "../modules/1804_kombucha_scoby_tracker/kombucha_scoby_tracker.hpp"

namespace {

testing::TestOutcome run_kombucha_scoby_tracker() {
    kombucha_scoby_tracker::init();
    auto s = kombucha_scoby_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    kombucha_scoby_tracker::shutdown();
    return testing::ok();
}

const int _reg_kombucha_scoby_tracker = testing::register_test(
    "kombucha_scoby_tracker",
    "1804_kombucha_scoby_tracker: stub status check",
    &run_kombucha_scoby_tracker);

}
