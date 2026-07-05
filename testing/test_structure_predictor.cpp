// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1327_structure_predictor/structure_predictor.hpp"

namespace {

testing::TestOutcome run_structure_predictor() {
    structure_predictor::init();
    auto s = structure_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    structure_predictor::shutdown();
    return testing::ok();
}

const int _reg_structure_predictor = testing::register_test(
    "structure_predictor",
    "1327_structure_predictor: stub status check",
    &run_structure_predictor);

}
