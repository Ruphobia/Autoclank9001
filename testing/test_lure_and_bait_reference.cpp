#include "test_runner.hpp"
#include "../modules/2047_lure_and_bait_reference/lure_and_bait_reference.hpp"

namespace {

testing::TestOutcome run_lure_and_bait_reference() {
    lure_and_bait_reference::init();
    auto s = lure_and_bait_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lure_and_bait_reference::shutdown();
    return testing::ok();
}

const int _reg_lure_and_bait_reference = testing::register_test(
    "lure_and_bait_reference",
    "2047_lure_and_bait_reference: stub status check",
    &run_lure_and_bait_reference);

}
