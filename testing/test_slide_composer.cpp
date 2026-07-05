// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/414_slide_composer/slide_composer.hpp"

namespace {

testing::TestOutcome run_slide_composer() {
    slide_composer::init();
    auto s = slide_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    slide_composer::shutdown();
    return testing::ok();
}

const int _reg_slide_composer = testing::register_test(
    "slide_composer",
    "414_slide_composer: stub status check",
    &run_slide_composer);

}
