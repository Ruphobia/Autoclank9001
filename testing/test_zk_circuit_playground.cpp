#include "test_runner.hpp"
#include "../modules/390_zk_circuit_playground/zk_circuit_playground.hpp"

namespace {

testing::TestOutcome run_zk_circuit_playground() {
    zk_circuit_playground::init();
    auto s = zk_circuit_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    zk_circuit_playground::shutdown();
    return testing::ok();
}

const int _reg_zk_circuit_playground = testing::register_test(
    "zk_circuit_playground",
    "390_zk_circuit_playground: stub status check",
    &run_zk_circuit_playground);

}
