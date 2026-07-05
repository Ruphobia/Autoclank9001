#include "test_runner.hpp"
#include "../modules/429_pivot_table_builder/pivot_table_builder.hpp"

namespace {

testing::TestOutcome run_pivot_table_builder() {
    pivot_table_builder::init();
    auto s = pivot_table_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pivot_table_builder::shutdown();
    return testing::ok();
}

const int _reg_pivot_table_builder = testing::register_test(
    "pivot_table_builder",
    "429_pivot_table_builder: stub status check",
    &run_pivot_table_builder);

}
