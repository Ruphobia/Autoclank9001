// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/637_redactor/redactor.hpp"

namespace {

testing::TestOutcome run_redactor() {
    redactor::init();
    auto s = redactor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    redactor::shutdown();
    return testing::ok();
}

const int _reg_redactor = testing::register_test(
    "redactor",
    "637_redactor: stub status check",
    &run_redactor);

}
