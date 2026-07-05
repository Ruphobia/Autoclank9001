// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 002_dictionary synonyms() (Moby Thesaurus II, public
// domain, resources/dictionary/mthesaur.txt).

#include "test_runner.hpp"
#include "../modules/002_dictionary/dictionary.hpp"

#include <filesystem>

namespace {

testing::TestOutcome run() {
    if (!std::filesystem::exists("resources/dictionary/mthesaur.txt"))
        return testing::skip("mthesaur.txt not present");

    auto sy = dictionary::synonyms("happy", 12);
    if (sy.empty())
        return testing::fail("no synonyms for 'happy'");
    if (sy.size() > 12)
        return testing::fail("max_results not honoured");
    bool plausible = false;
    for (const auto & s : sy)
        if (s == "accepting" || s == "apt" || s == "glad") plausible = true;
    if (!plausible)
        return testing::fail("unexpected synonym set for 'happy': " + sy[0]);

    // Case-insensitive root lookup.
    if (dictionary::synonyms("Happy", 3).size() != 3)
        return testing::fail("case-insensitive lookup failed");

    // Unknown words are empty, not an error.
    if (!dictionary::synonyms("zzzznotaword").empty())
        return testing::fail("nonexistent word returned synonyms");

    // max_results <= 0 returns the full list (well beyond the default cap).
    if (dictionary::synonyms("happy", 0).size() <= 12)
        return testing::fail("full-list mode did not exceed the default cap");

    return testing::ok();
}

const int _reg = testing::register_test(
    "thesaurus",
    "Moby synonyms(): lookup, result cap, case fold, graceful misses",
    &run);

}  // namespace
