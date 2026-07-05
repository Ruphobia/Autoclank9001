// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/216_usb_descriptor_builder/usb_descriptor_builder.hpp"

namespace {

testing::TestOutcome run_usb_descriptor_builder() {
    usb_descriptor_builder::init();
    auto s = usb_descriptor_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    usb_descriptor_builder::shutdown();
    return testing::ok();
}

const int _reg_usb_descriptor_builder = testing::register_test(
    "usb_descriptor_builder",
    "216_usb_descriptor_builder: stub status check",
    &run_usb_descriptor_builder);

}
