// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/726_multispectral_image_stack/multispectral_image_stack.hpp"

namespace {

testing::TestOutcome run_multispectral_image_stack() {
    multispectral_image_stack::init();
    auto s = multispectral_image_stack::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multispectral_image_stack::shutdown();
    return testing::ok();
}

const int _reg_multispectral_image_stack = testing::register_test(
    "multispectral_image_stack",
    "726_multispectral_image_stack: stub status check",
    &run_multispectral_image_stack);

}
