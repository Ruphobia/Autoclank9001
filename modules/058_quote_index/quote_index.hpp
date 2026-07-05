#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Quote and Passage Index: per-project SQLite + FTS5 store for
// highlighted passages and pulled quotes. Each quote carries a
// pointer back to its source (a 054_reference_library item id, a
// URL, or any free-form ref string), a locator (page / paragraph /
// section), the text itself, and short before / after context so the
// UI can render the passage in situ without re-fetching the source.
// Stored at <project>/.tool/quotes/quotes.sqlite.
namespace quote_index {

struct Quote {
    int64_t                  id            = 0;
    std::string              source_ref;     // "ref:42" / "url:https://..." / free-form
    std::string              locator;        // "p.5" / "§3.2" / "para 7"
    std::string              text;
    std::string              before;
    std::string              after;
    std::vector<std::string> tags;
    int64_t                  added_unix    = 0;
};

struct Hit {
    Quote        quote;
    double       score   = 0.0;
    std::string  snippet;
};

void open(std::string_view project_root);
void close();

int64_t add(const Quote & q);
void    update(int64_t id, const Quote & q);
void    forget(int64_t id);

std::optional<Quote>   get(int64_t id);
std::vector<Quote>     list_by_source(std::string_view source_ref, size_t limit = 100);
std::vector<Hit>       search(std::string_view query, size_t top_k = 8);

// Render a quote as a markdown blockquote with the locator and source
// noted underneath. Convenience for the "drop into a doc" UI affordance.
std::string format_markdown(const Quote & q);

}
