// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/406_repair_notebook/repair_notebook.hpp"

namespace {

testing::TestOutcome run_repair_notebook() {
    repair_notebook::init();
    auto s = repair_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    repair_notebook::shutdown();
    return testing::ok();
}

const int _reg_repair_notebook = testing::register_test(
    "repair_notebook",
    "406_repair_notebook: stub status check",
    &run_repair_notebook);

}
