#include "test_runner.hpp"
#include "../modules/1860_project_costing_and_stock_ledger/project_costing_and_stock_ledger.hpp"

namespace {

testing::TestOutcome run_project_costing_and_stock_ledger() {
    project_costing_and_stock_ledger::init();
    auto s = project_costing_and_stock_ledger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    project_costing_and_stock_ledger::shutdown();
    return testing::ok();
}

const int _reg_project_costing_and_stock_ledger = testing::register_test(
    "project_costing_and_stock_ledger",
    "1860_project_costing_and_stock_ledger: stub status check",
    &run_project_costing_and_stock_ledger);

}
