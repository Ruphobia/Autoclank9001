// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1146_rendezvous_proximity_ops/rendezvous_proximity_ops.hpp"

namespace {

testing::TestOutcome run_rendezvous_proximity_ops() {
    rendezvous_proximity_ops::init();
    auto s = rendezvous_proximity_ops::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rendezvous_proximity_ops::shutdown();
    return testing::ok();
}

const int _reg_rendezvous_proximity_ops = testing::register_test(
    "rendezvous_proximity_ops",
    "1146_rendezvous_proximity_ops: stub status check",
    &run_rendezvous_proximity_ops);

}
