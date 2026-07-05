// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/377_certificate_inspector/certificate_inspector.hpp"

namespace {

testing::TestOutcome run_certificate_inspector() {
    certificate_inspector::init();
    auto s = certificate_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    certificate_inspector::shutdown();
    return testing::ok();
}

const int _reg_certificate_inspector = testing::register_test(
    "certificate_inspector",
    "377_certificate_inspector: stub status check",
    &run_certificate_inspector);

}
