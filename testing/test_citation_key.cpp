// Smoke test for 055_citation_key.

#include "test_runner.hpp"
#include "../modules/055_citation_key/citation_key.hpp"

namespace {

testing::TestOutcome run() {
    nlohmann::json csl = {
        {"author", nlohmann::json::array({{{"family","Bauer"},{"given","Jane"}}})},
        {"issued", {{"date-parts", nlohmann::json::array({{2026}})}}},
        {"title", "Capybara thermoregulation in subtropical wetlands"},
        {"container-title", "Journal of Mammalogy"}
    };
    if (citation_key::generate(csl) != "bauer2026capybara")
        return testing::fail("default template wrong: " +
                             citation_key::generate(csl));
    if (citation_key::generate(csl, citation_key::Template::AuthorYear) != "bauer2026")
        return testing::fail("author-year template wrong");
    if (citation_key::generate(csl, citation_key::Template::AlphaThreeYear) != "Bau26")
        return testing::fail("alpha-3-year template wrong: " +
                             citation_key::generate(csl, citation_key::Template::AlphaThreeYear));

    // Journal short: "Journal of Mammalogy" -> drop "of" -> j m -> "jm"
    std::string k4 = citation_key::generate(csl, citation_key::Template::AuthorYearJournal);
    if (k4 != "bauer2026jm")
        return testing::fail("author-year-journal template wrong: " + k4);

    // Disambiguation cascade.
    std::unordered_set<std::string> taken = {"bauer2026capybara","bauer2026capybaraa"};
    auto disambiguated = citation_key::generate(csl,
        citation_key::Template::AuthorYearTitleWord, taken);
    if (disambiguated != "bauer2026capybarab")
        return testing::fail("disambiguation wrong: " + disambiguated);

    // Anonymous + no date.
    nlohmann::json bare = { {"title", "Just a stub note"} };
    auto kb = citation_key::generate(bare);
    if (kb.find("anon") == std::string::npos || kb.find("nodate") == std::string::npos)
        return testing::fail("anon fallback broken: " + kb);

    // sanitize_token + first_title_word helpers exposed.
    if (citation_key::sanitize_token("Foo, Bar! 9 ") != "foobar9")
        return testing::fail("sanitize_token broken");
    if (citation_key::first_title_word("The brown fox") != "brown")
        return testing::fail("first_title_word broken");

    return testing::ok();
}

const int _r = testing::register_test(
    "citation_key",
    "055_citation_key: BibTeX-style key templates + collision disambiguation",
    &run);

}
