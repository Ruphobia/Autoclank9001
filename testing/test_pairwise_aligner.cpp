// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1323_pairwise_aligner/pairwise_aligner.hpp"

namespace {

testing::TestOutcome run_pairwise_aligner() {
    pairwise_aligner::init();
    auto s = pairwise_aligner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pairwise_aligner::shutdown();
    return testing::ok();
}

const int _reg_pairwise_aligner = testing::register_test(
    "pairwise_aligner",
    "1323_pairwise_aligner: stub status check",
    &run_pairwise_aligner);

}
