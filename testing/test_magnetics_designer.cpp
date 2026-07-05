// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/863_magnetics_designer/magnetics_designer.hpp"

namespace {

testing::TestOutcome run_magnetics_designer() {
    magnetics_designer::init();
    auto s = magnetics_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    magnetics_designer::shutdown();
    return testing::ok();
}

const int _reg_magnetics_designer = testing::register_test(
    "magnetics_designer",
    "863_magnetics_designer: stub status check",
    &run_magnetics_designer);

}
