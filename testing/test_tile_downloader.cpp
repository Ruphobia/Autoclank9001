// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1597_tile_downloader/tile_downloader.hpp"

namespace {

testing::TestOutcome run_tile_downloader() {
    tile_downloader::init();
    auto s = tile_downloader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tile_downloader::shutdown();
    return testing::ok();
}

const int _reg_tile_downloader = testing::register_test(
    "tile_downloader",
    "1597_tile_downloader: stub status check",
    &run_tile_downloader);

}
