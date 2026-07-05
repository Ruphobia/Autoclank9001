// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/766_iconography_classifier/iconography_classifier.hpp"

namespace {

testing::TestOutcome run_iconography_classifier() {
    iconography_classifier::init();
    auto s = iconography_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    iconography_classifier::shutdown();
    return testing::ok();
}

const int _reg_iconography_classifier = testing::register_test(
    "iconography_classifier",
    "766_iconography_classifier: stub status check",
    &run_iconography_classifier);

}
