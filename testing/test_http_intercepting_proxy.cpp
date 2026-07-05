// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/138_http_intercepting_proxy/http_intercepting_proxy.hpp"

namespace {

testing::TestOutcome run_http_intercepting_proxy() {
    http_intercepting_proxy::init();
    auto s = http_intercepting_proxy::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    http_intercepting_proxy::shutdown();
    return testing::ok();
}

const int _reg_http_intercepting_proxy = testing::register_test(
    "http_intercepting_proxy",
    "138_http_intercepting_proxy: stub status check",
    &run_http_intercepting_proxy);

}
