// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1964_uci_engine_driver/uci_engine_driver.hpp"

namespace {

testing::TestOutcome run_uci_engine_driver() {
    uci_engine_driver::init();
    auto s = uci_engine_driver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    uci_engine_driver::shutdown();
    return testing::ok();
}

const int _reg_uci_engine_driver = testing::register_test(
    "uci_engine_driver",
    "1964_uci_engine_driver: stub status check",
    &run_uci_engine_driver);

}
