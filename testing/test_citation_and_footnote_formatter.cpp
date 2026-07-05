// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/691_citation_and_footnote_formatter/citation_and_footnote_formatter.hpp"

namespace {

testing::TestOutcome run_citation_and_footnote_formatter() {
    citation_and_footnote_formatter::init();
    auto s = citation_and_footnote_formatter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    citation_and_footnote_formatter::shutdown();
    return testing::ok();
}

const int _reg_citation_and_footnote_formatter = testing::register_test(
    "citation_and_footnote_formatter",
    "691_citation_and_footnote_formatter: stub status check",
    &run_citation_and_footnote_formatter);

}
