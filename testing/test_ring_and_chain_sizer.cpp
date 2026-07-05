#include "test_runner.hpp"
#include "../modules/1845_ring_and_chain_sizer/ring_and_chain_sizer.hpp"

namespace {

testing::TestOutcome run_ring_and_chain_sizer() {
    ring_and_chain_sizer::init();
    auto s = ring_and_chain_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ring_and_chain_sizer::shutdown();
    return testing::ok();
}

const int _reg_ring_and_chain_sizer = testing::register_test(
    "ring_and_chain_sizer",
    "1845_ring_and_chain_sizer: stub status check",
    &run_ring_and_chain_sizer);

}
