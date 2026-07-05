// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1403_dicom_viewer/dicom_viewer.hpp"

namespace {

testing::TestOutcome run_dicom_viewer() {
    dicom_viewer::init();
    auto s = dicom_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dicom_viewer::shutdown();
    return testing::ok();
}

const int _reg_dicom_viewer = testing::register_test(
    "dicom_viewer",
    "1403_dicom_viewer: stub status check",
    &run_dicom_viewer);

}
