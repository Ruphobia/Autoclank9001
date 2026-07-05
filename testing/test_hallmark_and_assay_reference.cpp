// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1856_hallmark_and_assay_reference/hallmark_and_assay_reference.hpp"

namespace {

testing::TestOutcome run_hallmark_and_assay_reference() {
    hallmark_and_assay_reference::init();
    auto s = hallmark_and_assay_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hallmark_and_assay_reference::shutdown();
    return testing::ok();
}

const int _reg_hallmark_and_assay_reference = testing::register_test(
    "hallmark_and_assay_reference",
    "1856_hallmark_and_assay_reference: stub status check",
    &run_hallmark_and_assay_reference);

}
