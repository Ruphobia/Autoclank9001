// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2092_enc_chart_renderer/enc_chart_renderer.hpp"

namespace {

testing::TestOutcome run_enc_chart_renderer() {
    enc_chart_renderer::init();
    auto s = enc_chart_renderer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    enc_chart_renderer::shutdown();
    return testing::ok();
}

const int _reg_enc_chart_renderer = testing::register_test(
    "enc_chart_renderer",
    "2092_enc_chart_renderer: stub status check",
    &run_enc_chart_renderer);

}
