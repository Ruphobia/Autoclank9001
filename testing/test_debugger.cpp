#include "test_runner.hpp"
#include "../modules/091_debugger/debugger.hpp"

namespace {

testing::TestOutcome run_debugger() {
    debugger::init();
    auto s = debugger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    debugger::shutdown();
    return testing::ok();
}

const int _reg_debugger = testing::register_test(
    "debugger",
    "091_debugger: stub status check",
    &run_debugger);

}
