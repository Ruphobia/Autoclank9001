// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2144_newspaper_and_obituary_miner/newspaper_and_obituary_miner.hpp"

namespace {

testing::TestOutcome run_newspaper_and_obituary_miner() {
    newspaper_and_obituary_miner::init();
    auto s = newspaper_and_obituary_miner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    newspaper_and_obituary_miner::shutdown();
    return testing::ok();
}

const int _reg_newspaper_and_obituary_miner = testing::register_test(
    "newspaper_and_obituary_miner",
    "2144_newspaper_and_obituary_miner: stub status check",
    &run_newspaper_and_obituary_miner);

}
