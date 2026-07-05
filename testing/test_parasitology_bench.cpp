// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1511_parasitology_bench/parasitology_bench.hpp"

namespace {

testing::TestOutcome run_parasitology_bench() {
    parasitology_bench::init();
    auto s = parasitology_bench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    parasitology_bench::shutdown();
    return testing::ok();
}

const int _reg_parasitology_bench = testing::register_test(
    "parasitology_bench",
    "1511_parasitology_bench: stub status check",
    &run_parasitology_bench);

}
