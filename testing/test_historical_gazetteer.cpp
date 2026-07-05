// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/693_historical_gazetteer/historical_gazetteer.hpp"

namespace {

testing::TestOutcome run_historical_gazetteer() {
    historical_gazetteer::init();
    auto s = historical_gazetteer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    historical_gazetteer::shutdown();
    return testing::ok();
}

const int _reg_historical_gazetteer = testing::register_test(
    "historical_gazetteer",
    "693_historical_gazetteer: stub status check",
    &run_historical_gazetteer);

}
