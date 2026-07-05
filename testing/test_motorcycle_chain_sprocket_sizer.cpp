// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1235_motorcycle_chain_sprocket_sizer/motorcycle_chain_sprocket_sizer.hpp"

namespace {

testing::TestOutcome run_motorcycle_chain_sprocket_sizer() {
    motorcycle_chain_sprocket_sizer::init();
    auto s = motorcycle_chain_sprocket_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    motorcycle_chain_sprocket_sizer::shutdown();
    return testing::ok();
}

const int _reg_motorcycle_chain_sprocket_sizer = testing::register_test(
    "motorcycle_chain_sprocket_sizer",
    "1235_motorcycle_chain_sprocket_sizer: stub status check",
    &run_motorcycle_chain_sprocket_sizer);

}
