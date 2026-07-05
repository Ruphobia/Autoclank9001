// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1211_pacejka_tire_model_lab/pacejka_tire_model_lab.hpp"

namespace {

testing::TestOutcome run_pacejka_tire_model_lab() {
    pacejka_tire_model_lab::init();
    auto s = pacejka_tire_model_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pacejka_tire_model_lab::shutdown();
    return testing::ok();
}

const int _reg_pacejka_tire_model_lab = testing::register_test(
    "pacejka_tire_model_lab",
    "1211_pacejka_tire_model_lab: stub status check",
    &run_pacejka_tire_model_lab);

}
