// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/144_tls_and_certificate_inspector/tls_and_certificate_inspector.hpp"

namespace {

testing::TestOutcome run_tls_and_certificate_inspector() {
    tls_and_certificate_inspector::init();
    auto s = tls_and_certificate_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tls_and_certificate_inspector::shutdown();
    return testing::ok();
}

const int _reg_tls_and_certificate_inspector = testing::register_test(
    "tls_and_certificate_inspector",
    "144_tls_and_certificate_inspector: stub status check",
    &run_tls_and_certificate_inspector);

}
