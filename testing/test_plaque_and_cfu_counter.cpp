// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1371_plaque_and_cfu_counter/plaque_and_cfu_counter.hpp"

namespace {

testing::TestOutcome run_plaque_and_cfu_counter() {
    plaque_and_cfu_counter::init();
    auto s = plaque_and_cfu_counter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plaque_and_cfu_counter::shutdown();
    return testing::ok();
}

const int _reg_plaque_and_cfu_counter = testing::register_test(
    "plaque_and_cfu_counter",
    "1371_plaque_and_cfu_counter: stub status check",
    &run_plaque_and_cfu_counter);

}
