// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/734_cross_tradition_concordance/cross_tradition_concordance.hpp"

namespace {

testing::TestOutcome run_cross_tradition_concordance() {
    cross_tradition_concordance::init();
    auto s = cross_tradition_concordance::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cross_tradition_concordance::shutdown();
    return testing::ok();
}

const int _reg_cross_tradition_concordance = testing::register_test(
    "cross_tradition_concordance",
    "734_cross_tradition_concordance: stub status check",
    &run_cross_tradition_concordance);

}
