// Smoke test for 037_recap_distiller.

#include "test_runner.hpp"
#include "../modules/037_recap_distiller/recap_distiller.hpp"

namespace {

testing::TestOutcome run() {
    using recap_distiller::Speaker;
    using recap_distiller::Turn;

    std::vector<Turn> transcript;
    transcript.push_back({Speaker::User,
        "Yeah, I prefer no em dashes. They look noisy.", 1, "sess-1"});
    transcript.push_back({Speaker::Assistant,
        "Noted. I will use colons instead.", 2, "sess-1"});
    transcript.push_back({Speaker::User,
        "Decided to use SQLite for the project memory backend.", 3, "sess-1"});
    transcript.push_back({Speaker::User,
        "TIL the dispenser stalls below 12 psi. Note that we observed this on lot 14.",
        4, "sess-1"});
    transcript.push_back({Speaker::User,
        "Just rambling here, nothing actionable.", 5, "sess-1"});

    auto cs = recap_distiller::distill(transcript);
    if (cs.empty()) return testing::fail("no candidates");

    auto count_kind = [&](memory_composer::EntryKind k){
        size_t n = 0;
        for (const auto & c : cs) if (c.draft.kind == k) ++n;
        return n;
    };
    if (count_kind(memory_composer::EntryKind::Preference) < 1)
        return testing::fail("preference not surfaced");
    if (count_kind(memory_composer::EntryKind::Decision) < 1)
        return testing::fail("decision not surfaced");
    if (count_kind(memory_composer::EntryKind::Fact) < 1)
        return testing::fail("fact not surfaced");

    bool seen_pref_em = false, seen_dec_sqlite = false, seen_fact_dispenser = false;
    for (const auto & c : cs) {
        if (c.draft.kind == memory_composer::EntryKind::Preference &&
            c.draft.content.find("em dashes") != std::string::npos) seen_pref_em = true;
        if (c.draft.kind == memory_composer::EntryKind::Decision &&
            c.draft.content.find("SQLite") != std::string::npos) seen_dec_sqlite = true;
        if (c.draft.kind == memory_composer::EntryKind::Fact &&
            c.draft.content.find("dispenser") != std::string::npos) seen_fact_dispenser = true;
        if (c.draft.source_link.find("session:sess-1:turn:") == std::string::npos)
            return testing::fail("source_link missing session-turn ref");
        if (c.draft.confidence < 0.5)
            return testing::fail("candidate confidence too low: " + std::to_string(c.draft.confidence));
    }
    if (!seen_pref_em)       return testing::fail("missed em dashes preference");
    if (!seen_dec_sqlite)    return testing::fail("missed SQLite decision");
    if (!seen_fact_dispenser) return testing::fail("missed dispenser fact");

    // Assistant turns are ignored.
    for (const auto & c : cs) {
        if (c.turn_index == 2)
            return testing::fail("assistant turn distilled");
    }
    // Turn 5 has no trigger; verify nothing came from it.
    for (const auto & c : cs) {
        if (c.turn_index == 5) return testing::fail("non-trigger turn distilled");
    }

    return testing::ok();
}

const int _r = testing::register_test(
    "recap_distiller",
    "037_recap_distiller: trigger-based candidate extraction from a session transcript",
    &run);

}
