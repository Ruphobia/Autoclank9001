#include "test_runner.hpp"
#include "../modules/389_abi_calldata_decoder/abi_calldata_decoder.hpp"

namespace {

testing::TestOutcome run_abi_calldata_decoder() {
    abi_calldata_decoder::init();
    auto s = abi_calldata_decoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    abi_calldata_decoder::shutdown();
    return testing::ok();
}

const int _reg_abi_calldata_decoder = testing::register_test(
    "abi_calldata_decoder",
    "389_abi_calldata_decoder: stub status check",
    &run_abi_calldata_decoder);

}
