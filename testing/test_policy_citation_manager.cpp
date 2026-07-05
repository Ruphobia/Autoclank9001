// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1449_policy_citation_manager/policy_citation_manager.hpp"

namespace {

testing::TestOutcome run_policy_citation_manager() {
    policy_citation_manager::init();
    auto s = policy_citation_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    policy_citation_manager::shutdown();
    return testing::ok();
}

const int _reg_policy_citation_manager = testing::register_test(
    "policy_citation_manager",
    "1449_policy_citation_manager: stub status check",
    &run_policy_citation_manager);

}
