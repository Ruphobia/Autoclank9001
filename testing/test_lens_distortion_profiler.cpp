// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/318_lens_distortion_profiler/lens_distortion_profiler.hpp"

namespace {

testing::TestOutcome run_lens_distortion_profiler() {
    lens_distortion_profiler::init();
    auto s = lens_distortion_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lens_distortion_profiler::shutdown();
    return testing::ok();
}

const int _reg_lens_distortion_profiler = testing::register_test(
    "lens_distortion_profiler",
    "318_lens_distortion_profiler: stub status check",
    &run_lens_distortion_profiler);

}
