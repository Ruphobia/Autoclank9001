// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/584_citation_formatter/citation_formatter.hpp"

namespace {

testing::TestOutcome run_citation_formatter() {
    citation_formatter::init();
    auto s = citation_formatter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    citation_formatter::shutdown();
    return testing::ok();
}

const int _reg_citation_formatter = testing::register_test(
    "citation_formatter",
    "584_citation_formatter: stub status check",
    &run_citation_formatter);

}
