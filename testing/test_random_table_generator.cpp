#include "test_runner.hpp"
#include "../modules/1951_random_table_generator/random_table_generator.hpp"

namespace {

testing::TestOutcome run_random_table_generator() {
    random_table_generator::init();
    auto s = random_table_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    random_table_generator::shutdown();
    return testing::ok();
}

const int _reg_random_table_generator = testing::register_test(
    "random_table_generator",
    "1951_random_table_generator: stub status check",
    &run_random_table_generator);

}
