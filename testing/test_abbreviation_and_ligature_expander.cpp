// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/727_abbreviation_and_ligature_expander/abbreviation_and_ligature_expander.hpp"

namespace {

testing::TestOutcome run_abbreviation_and_ligature_expander() {
    abbreviation_and_ligature_expander::init();
    auto s = abbreviation_and_ligature_expander::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    abbreviation_and_ligature_expander::shutdown();
    return testing::ok();
}

const int _reg_abbreviation_and_ligature_expander = testing::register_test(
    "abbreviation_and_ligature_expander",
    "727_abbreviation_and_ligature_expander: stub status check",
    &run_abbreviation_and_ligature_expander);

}
