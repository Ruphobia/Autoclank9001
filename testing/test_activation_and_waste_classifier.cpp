// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1108_activation_and_waste_classifier/activation_and_waste_classifier.hpp"

namespace {

testing::TestOutcome run_activation_and_waste_classifier() {
    activation_and_waste_classifier::init();
    auto s = activation_and_waste_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    activation_and_waste_classifier::shutdown();
    return testing::ok();
}

const int _reg_activation_and_waste_classifier = testing::register_test(
    "activation_and_waste_classifier",
    "1108_activation_and_waste_classifier: stub status check",
    &run_activation_and_waste_classifier);

}
