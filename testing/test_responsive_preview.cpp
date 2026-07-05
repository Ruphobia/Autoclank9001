// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/124_responsive_preview/responsive_preview.hpp"

namespace {

testing::TestOutcome run_responsive_preview() {
    responsive_preview::init();
    auto s = responsive_preview::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    responsive_preview::shutdown();
    return testing::ok();
}

const int _reg_responsive_preview = testing::register_test(
    "responsive_preview",
    "124_responsive_preview: stub status check",
    &run_responsive_preview);

}
