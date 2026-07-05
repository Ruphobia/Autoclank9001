// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1328_primer_designer/primer_designer.hpp"

namespace {

testing::TestOutcome run_primer_designer() {
    primer_designer::init();
    auto s = primer_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    primer_designer::shutdown();
    return testing::ok();
}

const int _reg_primer_designer = testing::register_test(
    "primer_designer",
    "1328_primer_designer: stub status check",
    &run_primer_designer);

}
