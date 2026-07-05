// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2134_y_dna_and_mtdna_haplogroup_explorer/y_dna_and_mtdna_haplogroup_explorer.hpp"

namespace {

testing::TestOutcome run_y_dna_and_mtdna_haplogroup_explorer() {
    y_dna_and_mtdna_haplogroup_explorer::init();
    auto s = y_dna_and_mtdna_haplogroup_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    y_dna_and_mtdna_haplogroup_explorer::shutdown();
    return testing::ok();
}

const int _reg_y_dna_and_mtdna_haplogroup_explorer = testing::register_test(
    "y_dna_and_mtdna_haplogroup_explorer",
    "2134_y_dna_and_mtdna_haplogroup_explorer: stub status check",
    &run_y_dna_and_mtdna_haplogroup_explorer);

}
