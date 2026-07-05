// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1528_phylogenetics_workbench/phylogenetics_workbench.hpp"

namespace {

testing::TestOutcome run_phylogenetics_workbench() {
    phylogenetics_workbench::init();
    auto s = phylogenetics_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    phylogenetics_workbench::shutdown();
    return testing::ok();
}

const int _reg_phylogenetics_workbench = testing::register_test(
    "phylogenetics_workbench",
    "1528_phylogenetics_workbench: stub status check",
    &run_phylogenetics_workbench);

}
