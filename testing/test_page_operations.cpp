// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/625_page_operations/page_operations.hpp"

namespace {

testing::TestOutcome run_page_operations() {
    page_operations::init();
    auto s = page_operations::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    page_operations::shutdown();
    return testing::ok();
}

const int _reg_page_operations = testing::register_test(
    "page_operations",
    "625_page_operations: stub status check",
    &run_page_operations);

}
