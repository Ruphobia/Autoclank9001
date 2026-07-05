// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/703_ocr_ground_truth_manager/ocr_ground_truth_manager.hpp"

namespace {

testing::TestOutcome run_ocr_ground_truth_manager() {
    ocr_ground_truth_manager::init();
    auto s = ocr_ground_truth_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ocr_ground_truth_manager::shutdown();
    return testing::ok();
}

const int _reg_ocr_ground_truth_manager = testing::register_test(
    "ocr_ground_truth_manager",
    "703_ocr_ground_truth_manager: stub status check",
    &run_ocr_ground_truth_manager);

}
