#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Project memory: per-project knowledge base accumulated over the life
// of the project. Distinct from 005_context (per-session chat log),
// 007_knowledge (Wikipedia), and 015_doc_reader (user documents): this
// is the short-form, AI-and-human-editable note store the pipeline
// reaches for when it needs "what did we already learn / decide /
// observe about this".
//
// Backed by SQLite (FTS5) at <project>/.tool/memory/project.sqlite.
namespace project_memory {

struct Entry {
    int64_t                  id          = 0;
    int64_t                  ts          = 0;     // unix seconds
    std::string              category;             // e.g. "observation", "preference", "fact"
    std::string              content;
    std::vector<std::string> tags;
    std::string              source;               // e.g. "chat:2026-06-29:turn-12", "manual", "file:..."
};

struct Hit {
    Entry        entry;
    double       score = 0.0;          // FTS5 bm25
    std::string  snippet;              // [[ ]] highlighted excerpt
};

void open(std::string_view project_root);
void close();

// Insert a new entry; returns its assigned id.
int64_t add(std::string_view              category,
            std::string_view              content,
            const std::vector<std::string> & tags    = {},
            std::string_view              source     = {});

// Replace content / tags / source on an existing entry. The category
// is not edited (use forget+add to recategorize).
void update(int64_t id, std::string_view content,
            const std::vector<std::string> & tags,
            std::string_view source);

void forget(int64_t id);

// List with simple equality filters; empty strings match anything.
std::vector<Entry> list(std::string_view category = {},
                        std::string_view tag      = {},
                        size_t           limit    = 100);

// Full-text search via FTS5. top_k caps the result count.
std::vector<Hit> search(std::string_view query, size_t top_k = 8);

std::optional<Entry> get(int64_t id);

}
