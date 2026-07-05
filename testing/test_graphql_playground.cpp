// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/135_graphql_playground/graphql_playground.hpp"

namespace {

testing::TestOutcome run_graphql_playground() {
    graphql_playground::init();
    auto s = graphql_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    graphql_playground::shutdown();
    return testing::ok();
}

const int _reg_graphql_playground = testing::register_test(
    "graphql_playground",
    "135_graphql_playground: stub status check",
    &run_graphql_playground);

}
