// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/731_provenance_and_dating_estimator/provenance_and_dating_estimator.hpp"

namespace {

testing::TestOutcome run_provenance_and_dating_estimator() {
    provenance_and_dating_estimator::init();
    auto s = provenance_and_dating_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    provenance_and_dating_estimator::shutdown();
    return testing::ok();
}

const int _reg_provenance_and_dating_estimator = testing::register_test(
    "provenance_and_dating_estimator",
    "731_provenance_and_dating_estimator: stub status check",
    &run_provenance_and_dating_estimator);

}
