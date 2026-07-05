#include "test_runner.hpp"
#include "../modules/670_vector_index_lab/vector_index_lab.hpp"

namespace {

testing::TestOutcome run_vector_index_lab() {
    vector_index_lab::init();
    auto s = vector_index_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vector_index_lab::shutdown();
    return testing::ok();
}

const int _reg_vector_index_lab = testing::register_test(
    "vector_index_lab",
    "670_vector_index_lab: stub status check",
    &run_vector_index_lab);

}
