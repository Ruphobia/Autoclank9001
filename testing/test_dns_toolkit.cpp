// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/140_dns_toolkit/dns_toolkit.hpp"

namespace {

testing::TestOutcome run_dns_toolkit() {
    dns_toolkit::init();
    auto s = dns_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dns_toolkit::shutdown();
    return testing::ok();
}

const int _reg_dns_toolkit = testing::register_test(
    "dns_toolkit",
    "140_dns_toolkit: stub status check",
    &run_dns_toolkit);

}
