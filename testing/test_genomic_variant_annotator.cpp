// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1415_genomic_variant_annotator/genomic_variant_annotator.hpp"

namespace {

testing::TestOutcome run_genomic_variant_annotator() {
    genomic_variant_annotator::init();
    auto s = genomic_variant_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    genomic_variant_annotator::shutdown();
    return testing::ok();
}

const int _reg_genomic_variant_annotator = testing::register_test(
    "genomic_variant_annotator",
    "1415_genomic_variant_annotator: stub status check",
    &run_genomic_variant_annotator);

}
