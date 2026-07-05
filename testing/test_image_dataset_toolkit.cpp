// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/682_image_dataset_toolkit/image_dataset_toolkit.hpp"

namespace {

testing::TestOutcome run_image_dataset_toolkit() {
    image_dataset_toolkit::init();
    auto s = image_dataset_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    image_dataset_toolkit::shutdown();
    return testing::ok();
}

const int _reg_image_dataset_toolkit = testing::register_test(
    "image_dataset_toolkit",
    "682_image_dataset_toolkit: stub status check",
    &run_image_dataset_toolkit);

}
