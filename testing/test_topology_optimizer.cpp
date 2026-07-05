// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/933_topology_optimizer/topology_optimizer.hpp"

namespace {

testing::TestOutcome run_topology_optimizer() {
    topology_optimizer::init();
    auto s = topology_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    topology_optimizer::shutdown();
    return testing::ok();
}

const int _reg_topology_optimizer = testing::register_test(
    "topology_optimizer",
    "933_topology_optimizer: stub status check",
    &run_topology_optimizer);

}
