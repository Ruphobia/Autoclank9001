// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/382_mnemonic_hd_wallet_studio/mnemonic_hd_wallet_studio.hpp"

namespace {

testing::TestOutcome run_mnemonic_hd_wallet_studio() {
    mnemonic_hd_wallet_studio::init();
    auto s = mnemonic_hd_wallet_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mnemonic_hd_wallet_studio::shutdown();
    return testing::ok();
}

const int _reg_mnemonic_hd_wallet_studio = testing::register_test(
    "mnemonic_hd_wallet_studio",
    "382_mnemonic_hd_wallet_studio: stub status check",
    &run_mnemonic_hd_wallet_studio);

}
