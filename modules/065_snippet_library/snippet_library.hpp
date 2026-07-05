#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Snippet library: tag-searchable store of reusable code, commands,
// queries, and prompts at <project>/.tool/snippets/snippets.sqlite.
// Snippets carry {{name}}-style placeholders the renderer substitutes
// at insert time, plus a free-form description and tag set for FTS5
// search.
namespace snippet_library {

struct Snippet {
    int64_t                  id = 0;
    std::string              name;
    std::string              language;     // "bash", "python", "sql", "prompt", ...
    std::string              body;
    std::string              description;
    std::vector<std::string> tags;
    std::vector<std::string> params;       // names extracted from {{...}} placeholders
};

struct Hit {
    Snippet     snippet;
    double      score   = 0.0;
    std::string snippet_text;     // FTS snippet excerpt
};

void open(std::string_view project_root);
void close();

int64_t add(const Snippet & s);
void    update(int64_t id, const Snippet & s);
void    forget(int64_t id);

std::optional<Snippet>      get(int64_t id);
std::vector<Snippet>        list_by_language(std::string_view lang, size_t limit = 100);
std::vector<Snippet>        list_by_tag(std::string_view tag,        size_t limit = 100);
std::vector<Hit>            search(std::string_view query,            size_t top_k = 8);

// Walk `body` and return the ordered list of unique placeholder names
// found in `{{name}}` markers.
std::vector<std::string> extract_params(std::string_view body);

// Render `body` substituting every `{{name}}` with `params[name]`.
// Missing keys leave the marker intact so the user can spot them.
std::string render(std::string_view body,
                   const std::map<std::string, std::string> & params);

}
