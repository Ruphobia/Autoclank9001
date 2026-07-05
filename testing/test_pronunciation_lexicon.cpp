// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/840_pronunciation_lexicon/pronunciation_lexicon.hpp"

namespace {

testing::TestOutcome run_pronunciation_lexicon() {
    pronunciation_lexicon::init();
    auto s = pronunciation_lexicon::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pronunciation_lexicon::shutdown();
    return testing::ok();
}

const int _reg_pronunciation_lexicon = testing::register_test(
    "pronunciation_lexicon",
    "840_pronunciation_lexicon: stub status check",
    &run_pronunciation_lexicon);

}
