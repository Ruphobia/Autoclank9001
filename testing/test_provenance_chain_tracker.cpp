// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/698_provenance_chain_tracker/provenance_chain_tracker.hpp"

namespace {

testing::TestOutcome run_provenance_chain_tracker() {
    provenance_chain_tracker::init();
    auto s = provenance_chain_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    provenance_chain_tracker::shutdown();
    return testing::ok();
}

const int _reg_provenance_chain_tracker = testing::register_test(
    "provenance_chain_tracker",
    "698_provenance_chain_tracker: stub status check",
    &run_provenance_chain_tracker);

}
