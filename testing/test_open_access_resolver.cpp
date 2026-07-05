// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/086_open_access_resolver/open_access_resolver.hpp"

namespace {

testing::TestOutcome run_open_access_resolver() {
    open_access_resolver::init();
    auto s = open_access_resolver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    open_access_resolver::shutdown();
    return testing::ok();
}

const int _reg_open_access_resolver = testing::register_test(
    "open_access_resolver",
    "086_open_access_resolver: stub status check",
    &run_open_access_resolver);

}
