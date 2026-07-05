// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/585_license_picker/license_picker.hpp"

namespace {

testing::TestOutcome run_license_picker() {
    license_picker::init();
    auto s = license_picker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    license_picker::shutdown();
    return testing::ok();
}

const int _reg_license_picker = testing::register_test(
    "license_picker",
    "585_license_picker: stub status check",
    &run_license_picker);

}
