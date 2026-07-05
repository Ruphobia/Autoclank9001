// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2151_wearable_data_importer/wearable_data_importer.hpp"

namespace {

testing::TestOutcome run_wearable_data_importer() {
    wearable_data_importer::init();
    auto s = wearable_data_importer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wearable_data_importer::shutdown();
    return testing::ok();
}

const int _reg_wearable_data_importer = testing::register_test(
    "wearable_data_importer",
    "2151_wearable_data_importer: stub status check",
    &run_wearable_data_importer);

}
