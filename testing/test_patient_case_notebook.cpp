// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1407_patient_case_notebook/patient_case_notebook.hpp"

namespace {

testing::TestOutcome run_patient_case_notebook() {
    patient_case_notebook::init();
    auto s = patient_case_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patient_case_notebook::shutdown();
    return testing::ok();
}

const int _reg_patient_case_notebook = testing::register_test(
    "patient_case_notebook",
    "1407_patient_case_notebook: stub status check",
    &run_patient_case_notebook);

}
