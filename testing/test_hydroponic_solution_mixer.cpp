// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1613_hydroponic_solution_mixer/hydroponic_solution_mixer.hpp"

namespace {

testing::TestOutcome run_hydroponic_solution_mixer() {
    hydroponic_solution_mixer::init();
    auto s = hydroponic_solution_mixer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hydroponic_solution_mixer::shutdown();
    return testing::ok();
}

const int _reg_hydroponic_solution_mixer = testing::register_test(
    "hydroponic_solution_mixer",
    "1613_hydroponic_solution_mixer: stub status check",
    &run_hydroponic_solution_mixer);

}
