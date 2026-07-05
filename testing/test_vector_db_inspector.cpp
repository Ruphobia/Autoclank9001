#include "test_runner.hpp"
#include "../modules/240_vector_db_inspector/vector_db_inspector.hpp"

namespace {

testing::TestOutcome run_vector_db_inspector() {
    vector_db_inspector::init();
    auto s = vector_db_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vector_db_inspector::shutdown();
    return testing::ok();
}

const int _reg_vector_db_inspector = testing::register_test(
    "vector_db_inspector",
    "240_vector_db_inspector: stub status check",
    &run_vector_db_inspector);

}
