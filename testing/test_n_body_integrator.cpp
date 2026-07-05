// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/913_n_body_integrator/n_body_integrator.hpp"

namespace {

testing::TestOutcome run_n_body_integrator() {
    n_body_integrator::init();
    auto s = n_body_integrator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    n_body_integrator::shutdown();
    return testing::ok();
}

const int _reg_n_body_integrator = testing::register_test(
    "n_body_integrator",
    "913_n_body_integrator: stub status check",
    &run_n_body_integrator);

}
