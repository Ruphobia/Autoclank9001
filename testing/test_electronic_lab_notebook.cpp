// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1307_electronic_lab_notebook/electronic_lab_notebook.hpp"

namespace {

testing::TestOutcome run_electronic_lab_notebook() {
    electronic_lab_notebook::init();
    auto s = electronic_lab_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    electronic_lab_notebook::shutdown();
    return testing::ok();
}

const int _reg_electronic_lab_notebook = testing::register_test(
    "electronic_lab_notebook",
    "1307_electronic_lab_notebook: stub status check",
    &run_electronic_lab_notebook);

}
