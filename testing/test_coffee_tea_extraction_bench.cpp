#include "test_runner.hpp"
#include "../modules/1801_coffee_tea_extraction_bench/coffee_tea_extraction_bench.hpp"

namespace {

testing::TestOutcome run_coffee_tea_extraction_bench() {
    coffee_tea_extraction_bench::init();
    auto s = coffee_tea_extraction_bench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    coffee_tea_extraction_bench::shutdown();
    return testing::ok();
}

const int _reg_coffee_tea_extraction_bench = testing::register_test(
    "coffee_tea_extraction_bench",
    "1801_coffee_tea_extraction_bench: stub status check",
    &run_coffee_tea_extraction_bench);

}
