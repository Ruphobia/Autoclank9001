// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1382_vaccine_antigen_designer/vaccine_antigen_designer.hpp"

namespace {

testing::TestOutcome run_vaccine_antigen_designer() {
    vaccine_antigen_designer::init();
    auto s = vaccine_antigen_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vaccine_antigen_designer::shutdown();
    return testing::ok();
}

const int _reg_vaccine_antigen_designer = testing::register_test(
    "vaccine_antigen_designer",
    "1382_vaccine_antigen_designer: stub status check",
    &run_vaccine_antigen_designer);

}
