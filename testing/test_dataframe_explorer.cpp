// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/655_dataframe_explorer/dataframe_explorer.hpp"

namespace {

testing::TestOutcome run_dataframe_explorer() {
    dataframe_explorer::init();
    auto s = dataframe_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dataframe_explorer::shutdown();
    return testing::ok();
}

const int _reg_dataframe_explorer = testing::register_test(
    "dataframe_explorer",
    "655_dataframe_explorer: stub status check",
    &run_dataframe_explorer);

}
