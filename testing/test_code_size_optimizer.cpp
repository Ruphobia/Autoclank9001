// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/205_code_size_optimizer/code_size_optimizer.hpp"

namespace {

testing::TestOutcome run_code_size_optimizer() {
    code_size_optimizer::init();
    auto s = code_size_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    code_size_optimizer::shutdown();
    return testing::ok();
}

const int _reg_code_size_optimizer = testing::register_test(
    "code_size_optimizer",
    "205_code_size_optimizer: stub status check",
    &run_code_size_optimizer);

}
