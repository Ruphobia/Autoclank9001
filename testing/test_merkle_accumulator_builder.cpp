#include "test_runner.hpp"
#include "../modules/391_merkle_accumulator_builder/merkle_accumulator_builder.hpp"

namespace {

testing::TestOutcome run_merkle_accumulator_builder() {
    merkle_accumulator_builder::init();
    auto s = merkle_accumulator_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    merkle_accumulator_builder::shutdown();
    return testing::ok();
}

const int _reg_merkle_accumulator_builder = testing::register_test(
    "merkle_accumulator_builder",
    "391_merkle_accumulator_builder: stub status check",
    &run_merkle_accumulator_builder);

}
