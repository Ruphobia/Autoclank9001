// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/170_privacy_manifest_generator/privacy_manifest_generator.hpp"

namespace {

testing::TestOutcome run_privacy_manifest_generator() {
    privacy_manifest_generator::init();
    auto s = privacy_manifest_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    privacy_manifest_generator::shutdown();
    return testing::ok();
}

const int _reg_privacy_manifest_generator = testing::register_test(
    "privacy_manifest_generator",
    "170_privacy_manifest_generator: stub status check",
    &run_privacy_manifest_generator);

}
