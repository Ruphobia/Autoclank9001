// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1691_asset_library/asset_library.hpp"

namespace {

testing::TestOutcome run_asset_library() {
    asset_library::init();
    auto s = asset_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    asset_library::shutdown();
    return testing::ok();
}

const int _reg_asset_library = testing::register_test(
    "asset_library",
    "1691_asset_library: stub status check",
    &run_asset_library);

}
