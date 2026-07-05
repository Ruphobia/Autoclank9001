#include "test_runner.hpp"
#include "../modules/611_transaction_monitor/transaction_monitor.hpp"

namespace {

testing::TestOutcome run_transaction_monitor() {
    transaction_monitor::init();
    auto s = transaction_monitor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    transaction_monitor::shutdown();
    return testing::ok();
}

const int _reg_transaction_monitor = testing::register_test(
    "transaction_monitor",
    "611_transaction_monitor: stub status check",
    &run_transaction_monitor);

}
