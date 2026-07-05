// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1359_taxonomic_classifier/taxonomic_classifier.hpp"

namespace {

testing::TestOutcome run_taxonomic_classifier() {
    taxonomic_classifier::init();
    auto s = taxonomic_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    taxonomic_classifier::shutdown();
    return testing::ok();
}

const int _reg_taxonomic_classifier = testing::register_test(
    "taxonomic_classifier",
    "1359_taxonomic_classifier: stub status check",
    &run_taxonomic_classifier);

}
