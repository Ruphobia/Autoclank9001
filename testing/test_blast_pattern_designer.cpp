// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1094_blast_pattern_designer/blast_pattern_designer.hpp"

namespace {

testing::TestOutcome run_blast_pattern_designer() {
    blast_pattern_designer::init();
    auto s = blast_pattern_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    blast_pattern_designer::shutdown();
    return testing::ok();
}

const int _reg_blast_pattern_designer = testing::register_test(
    "blast_pattern_designer",
    "1094_blast_pattern_designer: stub status check",
    &run_blast_pattern_designer);

}
