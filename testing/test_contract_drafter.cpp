// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/578_contract_drafter/contract_drafter.hpp"

namespace {

testing::TestOutcome run_contract_drafter() {
    contract_drafter::init();
    auto s = contract_drafter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    contract_drafter::shutdown();
    return testing::ok();
}

const int _reg_contract_drafter = testing::register_test(
    "contract_drafter",
    "578_contract_drafter: stub status check",
    &run_contract_drafter);

}
