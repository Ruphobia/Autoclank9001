// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1220_quarter_mile_predictor/quarter_mile_predictor.hpp"

namespace {

testing::TestOutcome run_quarter_mile_predictor() {
    quarter_mile_predictor::init();
    auto s = quarter_mile_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quarter_mile_predictor::shutdown();
    return testing::ok();
}

const int _reg_quarter_mile_predictor = testing::register_test(
    "quarter_mile_predictor",
    "1220_quarter_mile_predictor: stub status check",
    &run_quarter_mile_predictor);

}
