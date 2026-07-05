#include "test_runner.hpp"
#include "../modules/192_startup_and_vector_table_generator/startup_and_vector_table_generator.hpp"

namespace {

testing::TestOutcome run_startup_and_vector_table_generator() {
    startup_and_vector_table_generator::init();
    auto s = startup_and_vector_table_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    startup_and_vector_table_generator::shutdown();
    return testing::ok();
}

const int _reg_startup_and_vector_table_generator = testing::register_test(
    "startup_and_vector_table_generator",
    "192_startup_and_vector_table_generator: stub status check",
    &run_startup_and_vector_table_generator);

}
