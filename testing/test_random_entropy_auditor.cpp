// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/393_random_entropy_auditor/random_entropy_auditor.hpp"

namespace {

testing::TestOutcome run_random_entropy_auditor() {
    random_entropy_auditor::init();
    auto s = random_entropy_auditor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    random_entropy_auditor::shutdown();
    return testing::ok();
}

const int _reg_random_entropy_auditor = testing::register_test(
    "random_entropy_auditor",
    "393_random_entropy_auditor: stub status check",
    &run_random_entropy_auditor);

}
