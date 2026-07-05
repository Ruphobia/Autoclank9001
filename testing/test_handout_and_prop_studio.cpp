#include "test_runner.hpp"
#include "../modules/1961_handout_and_prop_studio/handout_and_prop_studio.hpp"

namespace {

testing::TestOutcome run_handout_and_prop_studio() {
    handout_and_prop_studio::init();
    auto s = handout_and_prop_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    handout_and_prop_studio::shutdown();
    return testing::ok();
}

const int _reg_handout_and_prop_studio = testing::register_test(
    "handout_and_prop_studio",
    "1961_handout_and_prop_studio: stub status check",
    &run_handout_and_prop_studio);

}
