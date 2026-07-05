// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1031_beam_designer/beam_designer.hpp"

namespace {

testing::TestOutcome run_beam_designer() {
    beam_designer::init();
    auto s = beam_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    beam_designer::shutdown();
    return testing::ok();
}

const int _reg_beam_designer = testing::register_test(
    "beam_designer",
    "1031_beam_designer: stub status check",
    &run_beam_designer);

}
