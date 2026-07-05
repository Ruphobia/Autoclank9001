// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/981_nesting_and_cut_list_optimizer/nesting_and_cut_list_optimizer.hpp"

namespace {

testing::TestOutcome run_nesting_and_cut_list_optimizer() {
    nesting_and_cut_list_optimizer::init();
    auto s = nesting_and_cut_list_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nesting_and_cut_list_optimizer::shutdown();
    return testing::ok();
}

const int _reg_nesting_and_cut_list_optimizer = testing::register_test(
    "nesting_and_cut_list_optimizer",
    "981_nesting_and_cut_list_optimizer: stub status check",
    &run_nesting_and_cut_list_optimizer);

}
