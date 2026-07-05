// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/758_lexicon_lookup/lexicon_lookup.hpp"

namespace {

testing::TestOutcome run_lexicon_lookup() {
    lexicon_lookup::init();
    auto s = lexicon_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lexicon_lookup::shutdown();
    return testing::ok();
}

const int _reg_lexicon_lookup = testing::register_test(
    "lexicon_lookup",
    "758_lexicon_lookup: stub status check",
    &run_lexicon_lookup);

}
