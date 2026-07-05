// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1722_style_corpus_search/style_corpus_search.hpp"

namespace {

testing::TestOutcome run_style_corpus_search() {
    style_corpus_search::init();
    auto s = style_corpus_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    style_corpus_search::shutdown();
    return testing::ok();
}

const int _reg_style_corpus_search = testing::register_test(
    "style_corpus_search",
    "1722_style_corpus_search: stub status check",
    &run_style_corpus_search);

}
