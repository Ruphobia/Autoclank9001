// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/237_mongodb_client/mongodb_client.hpp"

namespace {

testing::TestOutcome run_mongodb_client() {
    mongodb_client::init();
    auto s = mongodb_client::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mongodb_client::shutdown();
    return testing::ok();
}

const int _reg_mongodb_client = testing::register_test(
    "mongodb_client",
    "237_mongodb_client: stub status check",
    &run_mongodb_client);

}
