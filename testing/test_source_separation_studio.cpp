// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1735_source_separation_studio/source_separation_studio.hpp"

namespace {

testing::TestOutcome run_source_separation_studio() {
    source_separation_studio::init();
    auto s = source_separation_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    source_separation_studio::shutdown();
    return testing::ok();
}

const int _reg_source_separation_studio = testing::register_test(
    "source_separation_studio",
    "1735_source_separation_studio: stub status check",
    &run_source_separation_studio);

}
