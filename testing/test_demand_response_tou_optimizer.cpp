// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1294_demand_response_tou_optimizer/demand_response_tou_optimizer.hpp"

namespace {

testing::TestOutcome run_demand_response_tou_optimizer() {
    demand_response_tou_optimizer::init();
    auto s = demand_response_tou_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    demand_response_tou_optimizer::shutdown();
    return testing::ok();
}

const int _reg_demand_response_tou_optimizer = testing::register_test(
    "demand_response_tou_optimizer",
    "1294_demand_response_tou_optimizer: stub status check",
    &run_demand_response_tou_optimizer);

}
