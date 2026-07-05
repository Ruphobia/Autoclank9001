// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2085_scouting_notebook/scouting_notebook.hpp"

namespace {

testing::TestOutcome run_scouting_notebook() {
    scouting_notebook::init();
    auto s = scouting_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scouting_notebook::shutdown();
    return testing::ok();
}

const int _reg_scouting_notebook = testing::register_test(
    "scouting_notebook",
    "2085_scouting_notebook: stub status check",
    &run_scouting_notebook);

}
