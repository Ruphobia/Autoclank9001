#include "test_runner.hpp"
#include "../modules/459_scope_change_monitor/scope_change_monitor.hpp"

namespace {

testing::TestOutcome run_scope_change_monitor() {
    scope_change_monitor::init();
    auto s = scope_change_monitor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scope_change_monitor::shutdown();
    return testing::ok();
}

const int _reg_scope_change_monitor = testing::register_test(
    "scope_change_monitor",
    "459_scope_change_monitor: stub status check",
    &run_scope_change_monitor);

}
