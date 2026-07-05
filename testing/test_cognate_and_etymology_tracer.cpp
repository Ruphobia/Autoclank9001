// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/716_cognate_and_etymology_tracer/cognate_and_etymology_tracer.hpp"

namespace {

testing::TestOutcome run_cognate_and_etymology_tracer() {
    cognate_and_etymology_tracer::init();
    auto s = cognate_and_etymology_tracer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cognate_and_etymology_tracer::shutdown();
    return testing::ok();
}

const int _reg_cognate_and_etymology_tracer = testing::register_test(
    "cognate_and_etymology_tracer",
    "716_cognate_and_etymology_tracer: stub status check",
    &run_cognate_and_etymology_tracer);

}
