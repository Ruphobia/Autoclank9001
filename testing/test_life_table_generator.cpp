#include "test_runner.hpp"
#include "../modules/541_life_table_generator/life_table_generator.hpp"

namespace {

testing::TestOutcome run_life_table_generator() {
    life_table_generator::init();
    auto s = life_table_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    life_table_generator::shutdown();
    return testing::ok();
}

const int _reg_life_table_generator = testing::register_test(
    "life_table_generator",
    "541_life_table_generator: stub status check",
    &run_life_table_generator);

}
