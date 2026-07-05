// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2098_ais_decoder_and_viewer/ais_decoder_and_viewer.hpp"

namespace {

testing::TestOutcome run_ais_decoder_and_viewer() {
    ais_decoder_and_viewer::init();
    auto s = ais_decoder_and_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ais_decoder_and_viewer::shutdown();
    return testing::ok();
}

const int _reg_ais_decoder_and_viewer = testing::register_test(
    "ais_decoder_and_viewer",
    "2098_ais_decoder_and_viewer: stub status check",
    &run_ais_decoder_and_viewer);

}
