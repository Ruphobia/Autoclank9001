// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1988_patch_diff/patch_diff.hpp"

namespace {

testing::TestOutcome run_patch_diff() {
    patch_diff::init();
    auto s = patch_diff::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patch_diff::shutdown();
    return testing::ok();
}

const int _reg_patch_diff = testing::register_test(
    "patch_diff",
    "1988_patch_diff: stub status check",
    &run_patch_diff);

}
