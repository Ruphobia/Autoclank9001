// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/598_mrz_decoder/mrz_decoder.hpp"

namespace {

testing::TestOutcome run_mrz_decoder() {
    mrz_decoder::init();
    auto s = mrz_decoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mrz_decoder::shutdown();
    return testing::ok();
}

const int _reg_mrz_decoder = testing::register_test(
    "mrz_decoder",
    "598_mrz_decoder: stub status check",
    &run_mrz_decoder);

}
