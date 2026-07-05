// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2123_first_aid_decision_tree/first_aid_decision_tree.hpp"

namespace {

testing::TestOutcome run_first_aid_decision_tree() {
    first_aid_decision_tree::init();
    auto s = first_aid_decision_tree::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    first_aid_decision_tree::shutdown();
    return testing::ok();
}

const int _reg_first_aid_decision_tree = testing::register_test(
    "first_aid_decision_tree",
    "2123_first_aid_decision_tree: stub status check",
    &run_first_aid_decision_tree);

}
