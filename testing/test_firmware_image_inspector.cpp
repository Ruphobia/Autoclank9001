#include "test_runner.hpp"
#include "../modules/189_firmware_image_inspector/firmware_image_inspector.hpp"

namespace {

testing::TestOutcome run_firmware_image_inspector() {
    firmware_image_inspector::init();
    auto s = firmware_image_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    firmware_image_inspector::shutdown();
    return testing::ok();
}

const int _reg_firmware_image_inspector = testing::register_test(
    "firmware_image_inspector",
    "189_firmware_image_inspector: stub status check",
    &run_firmware_image_inspector);

}
