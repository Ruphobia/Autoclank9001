// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/214_edge_ml_deployer/edge_ml_deployer.hpp"

namespace {

testing::TestOutcome run_edge_ml_deployer() {
    edge_ml_deployer::init();
    auto s = edge_ml_deployer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    edge_ml_deployer::shutdown();
    return testing::ok();
}

const int _reg_edge_ml_deployer = testing::register_test(
    "edge_ml_deployer",
    "214_edge_ml_deployer: stub status check",
    &run_edge_ml_deployer);

}
