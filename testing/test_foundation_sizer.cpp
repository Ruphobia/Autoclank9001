// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1035_foundation_sizer/foundation_sizer.hpp"

namespace {

testing::TestOutcome run_foundation_sizer() {
    foundation_sizer::init();
    auto s = foundation_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    foundation_sizer::shutdown();
    return testing::ok();
}

const int _reg_foundation_sizer = testing::register_test(
    "foundation_sizer",
    "1035_foundation_sizer: stub status check",
    &run_foundation_sizer);

}
