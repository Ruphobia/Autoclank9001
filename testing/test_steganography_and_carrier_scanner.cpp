// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/369_steganography_and_carrier_scanner/steganography_and_carrier_scanner.hpp"

namespace {

testing::TestOutcome run_steganography_and_carrier_scanner() {
    steganography_and_carrier_scanner::init();
    auto s = steganography_and_carrier_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    steganography_and_carrier_scanner::shutdown();
    return testing::ok();
}

const int _reg_steganography_and_carrier_scanner = testing::register_test(
    "steganography_and_carrier_scanner",
    "369_steganography_and_carrier_scanner: stub status check",
    &run_steganography_and_carrier_scanner);

}
