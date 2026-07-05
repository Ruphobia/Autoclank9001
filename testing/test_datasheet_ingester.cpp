// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/202_datasheet_ingester/datasheet_ingester.hpp"

namespace {

testing::TestOutcome run_datasheet_ingester() {
    datasheet_ingester::init();
    auto s = datasheet_ingester::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    datasheet_ingester::shutdown();
    return testing::ok();
}

const int _reg_datasheet_ingester = testing::register_test(
    "datasheet_ingester",
    "202_datasheet_ingester: stub status check",
    &run_datasheet_ingester);

}
