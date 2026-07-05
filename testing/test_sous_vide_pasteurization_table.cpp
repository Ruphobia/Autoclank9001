// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1786_sous_vide_pasteurization_table/sous_vide_pasteurization_table.hpp"

namespace {

testing::TestOutcome run_sous_vide_pasteurization_table() {
    sous_vide_pasteurization_table::init();
    auto s = sous_vide_pasteurization_table::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sous_vide_pasteurization_table::shutdown();
    return testing::ok();
}

const int _reg_sous_vide_pasteurization_table = testing::register_test(
    "sous_vide_pasteurization_table",
    "1786_sous_vide_pasteurization_table: stub status check",
    &run_sous_vide_pasteurization_table);

}
