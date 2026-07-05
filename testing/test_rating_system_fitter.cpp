// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2086_rating_system_fitter/rating_system_fitter.hpp"

namespace {

testing::TestOutcome run_rating_system_fitter() {
    rating_system_fitter::init();
    auto s = rating_system_fitter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rating_system_fitter::shutdown();
    return testing::ok();
}

const int _reg_rating_system_fitter = testing::register_test(
    "rating_system_fitter",
    "2086_rating_system_fitter: stub status check",
    &run_rating_system_fitter);

}
