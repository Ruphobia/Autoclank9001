// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2103_magnetic_model/magnetic_model.hpp"

namespace {

testing::TestOutcome run_magnetic_model() {
    magnetic_model::init();
    auto s = magnetic_model::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    magnetic_model::shutdown();
    return testing::ok();
}

const int _reg_magnetic_model = testing::register_test(
    "magnetic_model",
    "2103_magnetic_model: stub status check",
    &run_magnetic_model);

}
