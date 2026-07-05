// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1183_cw_trainer_and_decoder/cw_trainer_and_decoder.hpp"

namespace {

testing::TestOutcome run_cw_trainer_and_decoder() {
    cw_trainer_and_decoder::init();
    auto s = cw_trainer_and_decoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cw_trainer_and_decoder::shutdown();
    return testing::ok();
}

const int _reg_cw_trainer_and_decoder = testing::register_test(
    "cw_trainer_and_decoder",
    "1183_cw_trainer_and_decoder: stub status check",
    &run_cw_trainer_and_decoder);

}
