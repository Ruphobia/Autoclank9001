#include "test_runner.hpp"
#include "../modules/1983_input_overlay/input_overlay.hpp"

namespace {

testing::TestOutcome run_input_overlay() {
    input_overlay::init();
    auto s = input_overlay::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    input_overlay::shutdown();
    return testing::ok();
}

const int _reg_input_overlay = testing::register_test(
    "input_overlay",
    "1983_input_overlay: stub status check",
    &run_input_overlay);

}
