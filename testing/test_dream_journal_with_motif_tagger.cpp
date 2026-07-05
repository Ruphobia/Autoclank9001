// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2153_dream_journal_with_motif_tagger/dream_journal_with_motif_tagger.hpp"

namespace {

testing::TestOutcome run_dream_journal_with_motif_tagger() {
    dream_journal_with_motif_tagger::init();
    auto s = dream_journal_with_motif_tagger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dream_journal_with_motif_tagger::shutdown();
    return testing::ok();
}

const int _reg_dream_journal_with_motif_tagger = testing::register_test(
    "dream_journal_with_motif_tagger",
    "2153_dream_journal_with_motif_tagger: stub status check",
    &run_dream_journal_with_motif_tagger);

}
