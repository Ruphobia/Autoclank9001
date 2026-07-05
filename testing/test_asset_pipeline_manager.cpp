// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/297_asset_pipeline_manager/asset_pipeline_manager.hpp"

namespace {

testing::TestOutcome run_asset_pipeline_manager() {
    asset_pipeline_manager::init();
    auto s = asset_pipeline_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    asset_pipeline_manager::shutdown();
    return testing::ok();
}

const int _reg_asset_pipeline_manager = testing::register_test(
    "asset_pipeline_manager",
    "297_asset_pipeline_manager: stub status check",
    &run_asset_pipeline_manager);

}
