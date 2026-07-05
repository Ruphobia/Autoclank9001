// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1345_antibody_and_epitope_designer/antibody_and_epitope_designer.hpp"

namespace {

testing::TestOutcome run_antibody_and_epitope_designer() {
    antibody_and_epitope_designer::init();
    auto s = antibody_and_epitope_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    antibody_and_epitope_designer::shutdown();
    return testing::ok();
}

const int _reg_antibody_and_epitope_designer = testing::register_test(
    "antibody_and_epitope_designer",
    "1345_antibody_and_epitope_designer: stub status check",
    &run_antibody_and_epitope_designer);

}
