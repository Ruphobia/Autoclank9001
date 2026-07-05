// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1828_cellarinventory/cellarinventory.hpp"

namespace {

testing::TestOutcome run_cellarinventory() {
    cellarinventory::init();
    auto s = cellarinventory::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cellarinventory::shutdown();
    return testing::ok();
}

const int _reg_cellarinventory = testing::register_test(
    "cellarinventory",
    "1828_cellarinventory: stub status check",
    &run_cellarinventory);

}
