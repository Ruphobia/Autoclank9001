// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1883_hallmark_and_purity_decoder/hallmark_and_purity_decoder.hpp"

namespace {

testing::TestOutcome run_hallmark_and_purity_decoder() {
    hallmark_and_purity_decoder::init();
    auto s = hallmark_and_purity_decoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hallmark_and_purity_decoder::shutdown();
    return testing::ok();
}

const int _reg_hallmark_and_purity_decoder = testing::register_test(
    "hallmark_and_purity_decoder",
    "1883_hallmark_and_purity_decoder: stub status check",
    &run_hallmark_and_purity_decoder);

}
