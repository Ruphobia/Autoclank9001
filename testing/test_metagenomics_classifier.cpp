// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1343_metagenomics_classifier/metagenomics_classifier.hpp"

namespace {

testing::TestOutcome run_metagenomics_classifier() {
    metagenomics_classifier::init();
    auto s = metagenomics_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    metagenomics_classifier::shutdown();
    return testing::ok();
}

const int _reg_metagenomics_classifier = testing::register_test(
    "metagenomics_classifier",
    "1343_metagenomics_classifier: stub status check",
    &run_metagenomics_classifier);

}
