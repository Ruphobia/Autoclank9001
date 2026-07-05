// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1513_studbook_and_pedigree_manager/studbook_and_pedigree_manager.hpp"

namespace {

testing::TestOutcome run_studbook_and_pedigree_manager() {
    studbook_and_pedigree_manager::init();
    auto s = studbook_and_pedigree_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    studbook_and_pedigree_manager::shutdown();
    return testing::ok();
}

const int _reg_studbook_and_pedigree_manager = testing::register_test(
    "studbook_and_pedigree_manager",
    "1513_studbook_and_pedigree_manager: stub status check",
    &run_studbook_and_pedigree_manager);

}
