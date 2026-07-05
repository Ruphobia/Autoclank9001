// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1438_microdata_explorer/microdata_explorer.hpp"

namespace {

testing::TestOutcome run_microdata_explorer() {
    microdata_explorer::init();
    auto s = microdata_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    microdata_explorer::shutdown();
    return testing::ok();
}

const int _reg_microdata_explorer = testing::register_test(
    "microdata_explorer",
    "1438_microdata_explorer: stub status check",
    &run_microdata_explorer);

}
