// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1331_crispr_guide_designer/crispr_guide_designer.hpp"

namespace {

testing::TestOutcome run_crispr_guide_designer() {
    crispr_guide_designer::init();
    auto s = crispr_guide_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crispr_guide_designer::shutdown();
    return testing::ok();
}

const int _reg_crispr_guide_designer = testing::register_test(
    "crispr_guide_designer",
    "1331_crispr_guide_designer: stub status check",
    &run_crispr_guide_designer);

}
