// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1792_wine_must_adjustment_bench/wine_must_adjustment_bench.hpp"

namespace {

testing::TestOutcome run_wine_must_adjustment_bench() {
    wine_must_adjustment_bench::init();
    auto s = wine_must_adjustment_bench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wine_must_adjustment_bench::shutdown();
    return testing::ok();
}

const int _reg_wine_must_adjustment_bench = testing::register_test(
    "wine_must_adjustment_bench",
    "1792_wine_must_adjustment_bench: stub status check",
    &run_wine_must_adjustment_bench);

}
