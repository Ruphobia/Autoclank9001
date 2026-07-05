// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2082_wearable_file_importer/wearable_file_importer.hpp"

namespace {

testing::TestOutcome run_wearable_file_importer() {
    wearable_file_importer::init();
    auto s = wearable_file_importer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wearable_file_importer::shutdown();
    return testing::ok();
}

const int _reg_wearable_file_importer = testing::register_test(
    "wearable_file_importer",
    "2082_wearable_file_importer: stub status check",
    &run_wearable_file_importer);

}
