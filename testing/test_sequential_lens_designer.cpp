// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/870_sequential_lens_designer/sequential_lens_designer.hpp"

namespace {

testing::TestOutcome run_sequential_lens_designer() {
    sequential_lens_designer::init();
    auto s = sequential_lens_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sequential_lens_designer::shutdown();
    return testing::ok();
}

const int _reg_sequential_lens_designer = testing::register_test(
    "sequential_lens_designer",
    "870_sequential_lens_designer: stub status check",
    &run_sequential_lens_designer);

}
