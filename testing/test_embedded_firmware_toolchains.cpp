// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/112_embedded_firmware_toolchains/embedded_firmware_toolchains.hpp"

namespace {

testing::TestOutcome run_embedded_firmware_toolchains() {
    embedded_firmware_toolchains::init();
    auto s = embedded_firmware_toolchains::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    embedded_firmware_toolchains::shutdown();
    return testing::ok();
}

const int _reg_embedded_firmware_toolchains = testing::register_test(
    "embedded_firmware_toolchains",
    "112_embedded_firmware_toolchains: stub status check",
    &run_embedded_firmware_toolchains);

}
