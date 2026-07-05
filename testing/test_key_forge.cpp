// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/371_key_forge/key_forge.hpp"

namespace {

testing::TestOutcome run_key_forge() {
    key_forge::init();
    auto s = key_forge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    key_forge::shutdown();
    return testing::ok();
}

const int _reg_key_forge = testing::register_test(
    "key_forge",
    "371_key_forge: stub status check",
    &run_key_forge);

}
