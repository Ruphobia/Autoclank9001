#include "test_runner.hpp"
#include "../modules/859_firmware_toolchain/firmware_toolchain.hpp"

namespace {

testing::TestOutcome run_firmware_toolchain() {
    firmware_toolchain::init();
    auto s = firmware_toolchain::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    firmware_toolchain::shutdown();
    return testing::ok();
}

const int _reg_firmware_toolchain = testing::register_test(
    "firmware_toolchain",
    "859_firmware_toolchain: stub status check",
    &run_firmware_toolchain);

}
