// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/919_data_importer/data_importer.hpp"

namespace {

testing::TestOutcome run_data_importer() {
    data_importer::init();
    auto s = data_importer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    data_importer::shutdown();
    return testing::ok();
}

const int _reg_data_importer = testing::register_test(
    "data_importer",
    "919_data_importer: stub status check",
    &run_data_importer);

}
