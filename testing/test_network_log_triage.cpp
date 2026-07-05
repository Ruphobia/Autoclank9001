// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/157_network_log_triage/network_log_triage.hpp"

namespace {

testing::TestOutcome run_network_log_triage() {
    network_log_triage::init();
    auto s = network_log_triage::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    network_log_triage::shutdown();
    return testing::ok();
}

const int _reg_network_log_triage = testing::register_test(
    "network_log_triage",
    "157_network_log_triage: stub status check",
    &run_network_log_triage);

}
