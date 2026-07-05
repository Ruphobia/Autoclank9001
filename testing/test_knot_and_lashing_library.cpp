// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2130_knot_and_lashing_library/knot_and_lashing_library.hpp"

namespace {

testing::TestOutcome run_knot_and_lashing_library() {
    knot_and_lashing_library::init();
    auto s = knot_and_lashing_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    knot_and_lashing_library::shutdown();
    return testing::ok();
}

const int _reg_knot_and_lashing_library = testing::register_test(
    "knot_and_lashing_library",
    "2130_knot_and_lashing_library: stub status check",
    &run_knot_and_lashing_library);

}
