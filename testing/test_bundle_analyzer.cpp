// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/121_bundle_analyzer/bundle_analyzer.hpp"

namespace {

testing::TestOutcome run_bundle_analyzer() {
    bundle_analyzer::init();
    auto s = bundle_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bundle_analyzer::shutdown();
    return testing::ok();
}

const int _reg_bundle_analyzer = testing::register_test(
    "bundle_analyzer",
    "121_bundle_analyzer: stub status check",
    &run_bundle_analyzer);

}
