// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/582_case_law_search/case_law_search.hpp"

namespace {

testing::TestOutcome run_case_law_search() {
    case_law_search::init();
    auto s = case_law_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    case_law_search::shutdown();
    return testing::ok();
}

const int _reg_case_law_search = testing::register_test(
    "case_law_search",
    "582_case_law_search: stub status check",
    &run_case_law_search);

}
