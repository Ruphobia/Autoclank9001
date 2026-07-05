#pragma once

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

// In-Text Citation Inserter: render a citation marker for a CSL-JSON
// item in the format the document host expects. Style controls the
// shape ((Author, Year), [1], (Bauer, 2026, p.5), etc.); Format
// controls the markup ([@key] for Pandoc, \cite{key} for LaTeX, raw
// text for Markdown / Plain, citet variants for biblatex when the
// caller asks for natbib_textual).
namespace citation_inserter {

enum class Style {
    AuthorDate          = 0,   // (Bauer, 2026)
    AuthorDateLocator   = 1,   // (Bauer, 2026, p.5)
    Numeric             = 2,   // [3] or \cite{key}
    Footnote            = 3,   // ^1 (host renders the footnote separately)
    NatbibTextual       = 4,   // Bauer (2026)  [for LaTeX natbib]
};
const char * to_string(Style);

enum class Format {
    Markdown = 0,    // plain "(Bauer, 2026)"
    Pandoc   = 1,    // "[@bauer2026]"
    LaTeX    = 2,    // "\cite{bauer2026}"
    Typst    = 3,    // "@bauer2026"
    Org      = 4,    // "[cite:@bauer2026]"
    Plain    = 5,
};
const char * to_string(Format);

void init();
void shutdown();

// Render the citation marker. `citation_key` is taken from
// item["id"] when item is supplied; otherwise pass the key explicitly
// via the overload below. `locator` (e.g. "p.5") is inserted where
// the style allows; prefix/suffix sit immediately before/after.
std::string render(const nlohmann::json & item,
                   Style  style,
                   Format format,
                   std::string_view locator = {},
                   std::string_view prefix  = {},
                   std::string_view suffix  = {});

std::string render_key(std::string_view citation_key,
                       std::string_view author_family,
                       std::string_view year,
                       Style  style,
                       Format format,
                       std::string_view locator = {},
                       std::string_view prefix  = {},
                       std::string_view suffix  = {});

}
