#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Knowledge Base Store: content-addressed store for fetched pages.
// Raw response bodies are written to <project>/.tool/kb/raw/<hash>;
// metadata (URL, host, fetched_at, mime, tags, extracted text) lives
// in a SQLite index at <project>/.tool/kb/kb.sqlite with FTS5 over
// the extracted text. Distinct from 015_doc_reader (user-supplied
// documents) and 022_project_memory (short typed notes): this is the
// crawler's bulk storage layer.
namespace kb_store {

struct Item {
    int64_t                  id          = 0;
    std::string              url;
    std::string              host;
    int64_t                  fetched_at  = 0;
    std::string              mime;
    std::string              content_hash;       // "fnv64:<hex>"
    std::vector<std::string> tags;
    size_t                   raw_bytes   = 0;
};

struct Hit {
    Item        item;
    double      score   = 0.0;       // bm25 (lower is better)
    std::string snippet;             // [[ ]]-highlighted excerpt
    std::string extracted_text;      // returned for inline display
};

void open(std::string_view project_root);
void close();

// Insert. Returns the assigned id. If a row with the same
// content_hash already exists, the existing id is returned and no
// duplicate raw blob is written.
int64_t ingest(std::string_view url,
               std::string_view host,
               int64_t          fetched_at,
               std::string_view mime,
               std::string_view raw_body,
               std::string_view extracted_text,
               const std::vector<std::string> & tags = {});

// Read back the raw bytes for `content_hash`. nullopt if absent.
std::optional<std::string> raw(std::string_view content_hash);

// Filtered listings.
std::vector<Item> list_by_url (std::string_view url,  size_t limit = 100);
std::vector<Item> list_by_host(std::string_view host, size_t limit = 100);
std::vector<Item> list_by_time(int64_t since_unix, int64_t until_unix,
                               size_t limit = 100);

// FTS over extracted_text.
std::vector<Hit> search(std::string_view query, size_t top_k = 8);

}
