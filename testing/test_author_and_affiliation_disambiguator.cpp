// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/651_author_and_affiliation_disambiguator/author_and_affiliation_disambiguator.hpp"

namespace {

testing::TestOutcome run_author_and_affiliation_disambiguator() {
    author_and_affiliation_disambiguator::init();
    auto s = author_and_affiliation_disambiguator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    author_and_affiliation_disambiguator::shutdown();
    return testing::ok();
}

const int _reg_author_and_affiliation_disambiguator = testing::register_test(
    "author_and_affiliation_disambiguator",
    "651_author_and_affiliation_disambiguator: stub status check",
    &run_author_and_affiliation_disambiguator);

}
