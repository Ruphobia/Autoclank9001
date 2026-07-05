#include "test_runner.hpp"
#include "../modules/398_bios_and_firmware_catalog/bios_and_firmware_catalog.hpp"

namespace {

testing::TestOutcome run_bios_and_firmware_catalog() {
    bios_and_firmware_catalog::init();
    auto s = bios_and_firmware_catalog::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bios_and_firmware_catalog::shutdown();
    return testing::ok();
}

const int _reg_bios_and_firmware_catalog = testing::register_test(
    "bios_and_firmware_catalog",
    "398_bios_and_firmware_catalog: stub status check",
    &run_bios_and_firmware_catalog);

}
