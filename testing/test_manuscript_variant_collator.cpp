// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/753_manuscript_variant_collator/manuscript_variant_collator.hpp"

namespace {

testing::TestOutcome run_manuscript_variant_collator() {
    manuscript_variant_collator::init();
    auto s = manuscript_variant_collator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    manuscript_variant_collator::shutdown();
    return testing::ok();
}

const int _reg_manuscript_variant_collator = testing::register_test(
    "manuscript_variant_collator",
    "753_manuscript_variant_collator: stub status check",
    &run_manuscript_variant_collator);

}
