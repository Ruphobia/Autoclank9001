// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1013_absorber_and_diffuser_designer/absorber_and_diffuser_designer.hpp"

namespace {

testing::TestOutcome run_absorber_and_diffuser_designer() {
    absorber_and_diffuser_designer::init();
    auto s = absorber_and_diffuser_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    absorber_and_diffuser_designer::shutdown();
    return testing::ok();
}

const int _reg_absorber_and_diffuser_designer = testing::register_test(
    "absorber_and_diffuser_designer",
    "1013_absorber_and_diffuser_designer: stub status check",
    &run_absorber_and_diffuser_designer);

}
