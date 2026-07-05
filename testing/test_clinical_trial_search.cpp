// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1405_clinical_trial_search/clinical_trial_search.hpp"

namespace {

testing::TestOutcome run_clinical_trial_search() {
    clinical_trial_search::init();
    auto s = clinical_trial_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    clinical_trial_search::shutdown();
    return testing::ok();
}

const int _reg_clinical_trial_search = testing::register_test(
    "clinical_trial_search",
    "1405_clinical_trial_search: stub status check",
    &run_clinical_trial_search);

}
