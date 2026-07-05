#include "test_runner.hpp"
#include "../modules/330_circuit_builder/circuit_builder.hpp"

namespace {

testing::TestOutcome run_circuit_builder() {
    circuit_builder::init();
    auto s = circuit_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    circuit_builder::shutdown();
    return testing::ok();
}

const int _reg_circuit_builder = testing::register_test(
    "circuit_builder",
    "330_circuit_builder: stub status check",
    &run_circuit_builder);

}
