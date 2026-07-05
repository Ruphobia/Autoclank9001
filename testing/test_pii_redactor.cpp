// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/613_pii_redactor/pii_redactor.hpp"

namespace {

testing::TestOutcome run_pii_redactor() {
    pii_redactor::init();
    auto s = pii_redactor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pii_redactor::shutdown();
    return testing::ok();
}

const int _reg_pii_redactor = testing::register_test(
    "pii_redactor",
    "613_pii_redactor: stub status check",
    &run_pii_redactor);

}
