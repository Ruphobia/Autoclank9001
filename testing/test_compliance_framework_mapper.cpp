// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/595_compliance_framework_mapper/compliance_framework_mapper.hpp"

namespace {

testing::TestOutcome run_compliance_framework_mapper() {
    compliance_framework_mapper::init();
    auto s = compliance_framework_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    compliance_framework_mapper::shutdown();
    return testing::ok();
}

const int _reg_compliance_framework_mapper = testing::register_test(
    "compliance_framework_mapper",
    "595_compliance_framework_mapper: stub status check",
    &run_compliance_framework_mapper);

}
