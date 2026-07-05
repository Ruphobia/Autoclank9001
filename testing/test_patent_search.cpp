// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/590_patent_search/patent_search.hpp"

namespace {

testing::TestOutcome run_patent_search() {
    patent_search::init();
    auto s = patent_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patent_search::shutdown();
    return testing::ok();
}

const int _reg_patent_search = testing::register_test(
    "patent_search",
    "590_patent_search: stub status check",
    &run_patent_search);

}
