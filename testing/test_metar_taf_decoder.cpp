// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1167_metar_taf_decoder/metar_taf_decoder.hpp"

namespace {

testing::TestOutcome run_metar_taf_decoder() {
    metar_taf_decoder::init();
    auto s = metar_taf_decoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    metar_taf_decoder::shutdown();
    return testing::ok();
}

const int _reg_metar_taf_decoder = testing::register_test(
    "metar_taf_decoder",
    "1167_metar_taf_decoder: stub status check",
    &run_metar_taf_decoder);

}
