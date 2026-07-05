// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1004_x_3d_print_material_picker/x_3d_print_material_picker.hpp"

namespace {

testing::TestOutcome run_x_3d_print_material_picker() {
    x_3d_print_material_picker::init();
    auto s = x_3d_print_material_picker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    x_3d_print_material_picker::shutdown();
    return testing::ok();
}

const int _reg_x_3d_print_material_picker = testing::register_test(
    "x_3d_print_material_picker",
    "1004_x_3d_print_material_picker: stub status check",
    &run_x_3d_print_material_picker);

}
