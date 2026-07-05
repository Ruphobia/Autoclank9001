// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1697_path_tracer_preview/path_tracer_preview.hpp"

namespace {

testing::TestOutcome run_path_tracer_preview() {
    path_tracer_preview::init();
    auto s = path_tracer_preview::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    path_tracer_preview::shutdown();
    return testing::ok();
}

const int _reg_path_tracer_preview = testing::register_test(
    "path_tracer_preview",
    "1697_path_tracer_preview: stub status check",
    &run_path_tracer_preview);

}
