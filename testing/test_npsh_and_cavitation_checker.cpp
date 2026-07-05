// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1022_npsh_and_cavitation_checker/npsh_and_cavitation_checker.hpp"

namespace {

testing::TestOutcome run_npsh_and_cavitation_checker() {
    npsh_and_cavitation_checker::init();
    auto s = npsh_and_cavitation_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    npsh_and_cavitation_checker::shutdown();
    return testing::ok();
}

const int _reg_npsh_and_cavitation_checker = testing::register_test(
    "npsh_and_cavitation_checker",
    "1022_npsh_and_cavitation_checker: stub status check",
    &run_npsh_and_cavitation_checker);

}
