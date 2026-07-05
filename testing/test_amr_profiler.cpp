// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1357_amr_profiler/amr_profiler.hpp"

namespace {

testing::TestOutcome run_amr_profiler() {
    amr_profiler::init();
    auto s = amr_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    amr_profiler::shutdown();
    return testing::ok();
}

const int _reg_amr_profiler = testing::register_test(
    "amr_profiler",
    "1357_amr_profiler: stub status check",
    &run_amr_profiler);

}
