// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/700_historical_units_converter/historical_units_converter.hpp"

namespace {

testing::TestOutcome run_historical_units_converter() {
    historical_units_converter::init();
    auto s = historical_units_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    historical_units_converter::shutdown();
    return testing::ok();
}

const int _reg_historical_units_converter = testing::register_test(
    "historical_units_converter",
    "700_historical_units_converter: stub status check",
    &run_historical_units_converter);

}
