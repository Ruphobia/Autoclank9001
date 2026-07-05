#include "test_runner.hpp"
#include "../modules/2089_body_composition_tracker/body_composition_tracker.hpp"

namespace {

testing::TestOutcome run_body_composition_tracker() {
    body_composition_tracker::init();
    auto s = body_composition_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    body_composition_tracker::shutdown();
    return testing::ok();
}

const int _reg_body_composition_tracker = testing::register_test(
    "body_composition_tracker",
    "2089_body_composition_tracker: stub status check",
    &run_body_composition_tracker);

}
