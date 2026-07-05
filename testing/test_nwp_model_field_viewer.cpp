// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1456_nwp_model_field_viewer/nwp_model_field_viewer.hpp"

namespace {

testing::TestOutcome run_nwp_model_field_viewer() {
    nwp_model_field_viewer::init();
    auto s = nwp_model_field_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nwp_model_field_viewer::shutdown();
    return testing::ok();
}

const int _reg_nwp_model_field_viewer = testing::register_test(
    "nwp_model_field_viewer",
    "1456_nwp_model_field_viewer: stub status check",
    &run_nwp_model_field_viewer);

}
