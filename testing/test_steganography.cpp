// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1663_steganography/steganography.hpp"

namespace {

testing::TestOutcome run_steganography() {
    steganography::init();
    auto s = steganography::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    steganography::shutdown();
    return testing::ok();
}

const int _reg_steganography = testing::register_test(
    "steganography",
    "1663_steganography: stub status check",
    &run_steganography);

}
