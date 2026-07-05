#include "test_runner.hpp"
#include "../modules/1955_gm_screen_reference/gm_screen_reference.hpp"

namespace {

testing::TestOutcome run_gm_screen_reference() {
    gm_screen_reference::init();
    auto s = gm_screen_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gm_screen_reference::shutdown();
    return testing::ok();
}

const int _reg_gm_screen_reference = testing::register_test(
    "gm_screen_reference",
    "1955_gm_screen_reference: stub status check",
    &run_gm_screen_reference);

}
