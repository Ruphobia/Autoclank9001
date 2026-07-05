// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1326_protein_structure_viewer/protein_structure_viewer.hpp"

namespace {

testing::TestOutcome run_protein_structure_viewer() {
    protein_structure_viewer::init();
    auto s = protein_structure_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    protein_structure_viewer::shutdown();
    return testing::ok();
}

const int _reg_protein_structure_viewer = testing::register_test(
    "protein_structure_viewer",
    "1326_protein_structure_viewer: stub status check",
    &run_protein_structure_viewer);

}
