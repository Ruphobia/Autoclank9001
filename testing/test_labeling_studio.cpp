// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/668_labeling_studio/labeling_studio.hpp"

namespace {

testing::TestOutcome run_labeling_studio() {
    labeling_studio::init();
    auto s = labeling_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    labeling_studio::shutdown();
    return testing::ok();
}

const int _reg_labeling_studio = testing::register_test(
    "labeling_studio",
    "668_labeling_studio: stub status check",
    &run_labeling_studio);

}
