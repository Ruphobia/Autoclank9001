#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Semantic Recall Probe: hybrid query over the project / personal
// memory stores. The FTS5 search in 022 / 023 supplies the candidate
// pool; this module re-ranks with a trigram-Jaccard similarity over
// the query and adds a short human-readable gloss explaining why each
// entry made the cut. When a real embedding model arrives, the
// trigram component is the slot it drops into; the API stays the same.
namespace recall_probe {

enum class Scope { Project = 0, Personal = 1 };

struct Hit {
    int64_t      entry_id   = 0;
    std::string  content;
    std::string  category;
    std::vector<std::string> tags;
    double       combined   = 0.0;
    double       fts_score  = 0.0;     // raw bm25 (lower is better)
    double       trigram    = 0.0;     // 0..1
    std::string  gloss;                 // why this matched (human-readable)
    std::string  snippet;               // FTS5 snippet with [[ ]] highlights
};

void init();
void shutdown();

// Top-k hybrid hits. `fts_pool` controls how many FTS candidates feed
// the trigram rerank; larger pools find more wide-net trigram matches
// at the cost of latency. Caller is expected to have opened the
// matching store (project_memory::open or personal_memory::open).
std::vector<Hit> probe(Scope scope, std::string_view query,
                       size_t top_k = 8, size_t fts_pool = 32);

}
