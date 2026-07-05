// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1539_tide_predictor/tide_predictor.hpp"

namespace {

testing::TestOutcome run_tide_predictor() {
    tide_predictor::init();
    auto s = tide_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tide_predictor::shutdown();
    return testing::ok();
}

const int _reg_tide_predictor = testing::register_test(
    "tide_predictor",
    "1539_tide_predictor: stub status check",
    &run_tide_predictor);

}
