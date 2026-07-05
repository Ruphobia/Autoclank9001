// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/728_patristic_citation_tracer/patristic_citation_tracer.hpp"

namespace {

testing::TestOutcome run_patristic_citation_tracer() {
    patristic_citation_tracer::init();
    auto s = patristic_citation_tracer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patristic_citation_tracer::shutdown();
    return testing::ok();
}

const int _reg_patristic_citation_tracer = testing::register_test(
    "patristic_citation_tracer",
    "728_patristic_citation_tracer: stub status check",
    &run_patristic_citation_tracer);

}
