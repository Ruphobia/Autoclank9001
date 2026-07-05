// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/826_language_and_script_identifier/language_and_script_identifier.hpp"

namespace {

testing::TestOutcome run_language_and_script_identifier() {
    language_and_script_identifier::init();
    auto s = language_and_script_identifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    language_and_script_identifier::shutdown();
    return testing::ok();
}

const int _reg_language_and_script_identifier = testing::register_test(
    "language_and_script_identifier",
    "826_language_and_script_identifier: stub status check",
    &run_language_and_script_identifier);

}
