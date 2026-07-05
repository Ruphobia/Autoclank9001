#include "test_runner.hpp"
#include "../modules/096_repl_and_scratchpad/repl_and_scratchpad.hpp"

namespace {

testing::TestOutcome run_repl_and_scratchpad() {
    repl_and_scratchpad::init();
    auto s = repl_and_scratchpad::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    repl_and_scratchpad::shutdown();
    return testing::ok();
}

const int _reg_repl_and_scratchpad = testing::register_test(
    "repl_and_scratchpad",
    "096_repl_and_scratchpad: stub status check",
    &run_repl_and_scratchpad);

}
