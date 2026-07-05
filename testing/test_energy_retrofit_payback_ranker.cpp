// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2030_energy_retrofit_payback_ranker/energy_retrofit_payback_ranker.hpp"

namespace {

testing::TestOutcome run_energy_retrofit_payback_ranker() {
    energy_retrofit_payback_ranker::init();
    auto s = energy_retrofit_payback_ranker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    energy_retrofit_payback_ranker::shutdown();
    return testing::ok();
}

const int _reg_energy_retrofit_payback_ranker = testing::register_test(
    "energy_retrofit_payback_ranker",
    "2030_energy_retrofit_payback_ranker: stub status check",
    &run_energy_retrofit_payback_ranker);

}
