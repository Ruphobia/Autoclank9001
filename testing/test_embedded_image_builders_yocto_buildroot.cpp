// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/113_embedded_image_builders_yocto_buildroot/embedded_image_builders_yocto_buildroot.hpp"

namespace {

testing::TestOutcome run_embedded_image_builders_yocto_buildroot() {
    embedded_image_builders_yocto_buildroot::init();
    auto s = embedded_image_builders_yocto_buildroot::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    embedded_image_builders_yocto_buildroot::shutdown();
    return testing::ok();
}

const int _reg_embedded_image_builders_yocto_buildroot = testing::register_test(
    "embedded_image_builders_yocto_buildroot",
    "113_embedded_image_builders_yocto_buildroot: stub status check",
    &run_embedded_image_builders_yocto_buildroot);

}
