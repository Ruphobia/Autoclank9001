#pragma once

#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <nlohmann/json.hpp>

// Citation Key Generator: rule-based BibTeX-style key builder with
// templated formats and collision disambiguation. Pure-function
// surface; works on raw CSL-JSON records so it does not require an
// open 054_reference_library.
namespace citation_key {

enum class Template {
    AuthorYearTitleWord = 0,   // bauer2026capybara
    AuthorYear          = 1,   // bauer2026
    AlphaThreeYear      = 2,   // Bau26 (Author short + 2-digit year)
    AuthorYearJournal   = 3,   // bauer2026jmamm
};
const char * to_string(Template);

// Build a key for `csl` using `tmpl`. If the resulting key is already
// in `existing`, a single-letter suffix (a, b, c, ..., z, aa, ab, ...)
// is appended until uniqueness is reached. The disambiguator does not
// consult the library; pass the set of already-taken keys explicitly
// so the caller controls scope.
std::string generate(const nlohmann::json & csl,
                     Template tmpl                 = Template::AuthorYearTitleWord,
                     const std::unordered_set<std::string> & existing = {});

// Lowercase ASCII fold + drop punctuation; intermediate helper exposed
// for callers that want to compose their own templates.
std::string sanitize_token(std::string_view in);

// Pull the first author's family name (preferred) or "anon" if absent.
std::string first_author_family(const nlohmann::json & csl);

// Take the first content word (longer than 3 chars, not a stoplist
// word) from `title`. Empty if title is missing.
std::string first_title_word(std::string_view title);

}
