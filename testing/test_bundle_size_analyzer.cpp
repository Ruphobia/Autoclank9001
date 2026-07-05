// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/165_bundle_size_analyzer/bundle_size_analyzer.hpp"

namespace {

testing::TestOutcome run_bundle_size_analyzer() {
    bundle_size_analyzer::init();
    auto s = bundle_size_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bundle_size_analyzer::shutdown();
    return testing::ok();
}

const int _reg_bundle_size_analyzer = testing::register_test(
    "bundle_size_analyzer",
    "165_bundle_size_analyzer: stub status check",
    &run_bundle_size_analyzer);

}
