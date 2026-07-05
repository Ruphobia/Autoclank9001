// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1532_ceramic_seriation/ceramic_seriation.hpp"

namespace {

testing::TestOutcome run_ceramic_seriation() {
    ceramic_seriation::init();
    auto s = ceramic_seriation::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ceramic_seriation::shutdown();
    return testing::ok();
}

const int _reg_ceramic_seriation = testing::register_test(
    "ceramic_seriation",
    "1532_ceramic_seriation: stub status check",
    &run_ceramic_seriation);

}
