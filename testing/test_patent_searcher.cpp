// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/573_patent_searcher/patent_searcher.hpp"

namespace {

testing::TestOutcome run_patent_searcher() {
    patent_searcher::init();
    auto s = patent_searcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patent_searcher::shutdown();
    return testing::ok();
}

const int _reg_patent_searcher = testing::register_test(
    "patent_searcher",
    "573_patent_searcher: stub status check",
    &run_patent_searcher);

}
