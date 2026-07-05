#include "test_runner.hpp"
#include "../modules/235_replication_monitor/replication_monitor.hpp"

namespace {

testing::TestOutcome run_replication_monitor() {
    replication_monitor::init();
    auto s = replication_monitor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    replication_monitor::shutdown();
    return testing::ok();
}

const int _reg_replication_monitor = testing::register_test(
    "replication_monitor",
    "235_replication_monitor: stub status check",
    &run_replication_monitor);

}
