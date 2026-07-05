#pragma once

#include "wikipedia.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace kb {

struct LookupResult {
    bool                  available = false;  // false if KB hasn't mounted a ZIM yet
    std::vector<WikiHit>  hits;
};

// Initialise the KB layer:
//  - if 007_knowledge/wikipedia.zim exists, open it for queries
//  - if missing OR no resume state, spawn a background downloader that
//    fetches Kiwix's current English-Wikipedia ZIM (no pictures), with
//    parallel chunked transfers and per-chunk resume state
//  - safe to call regardless of network availability; never blocks
//
// data_dir defaults to "resources/wikipedia".
void init(const std::string & data_dir = "resources/wikipedia");

// Stop the background downloader (if running) and close the ZIM.
void shutdown();

// Best-effort fuzzy entity lookup. Returns available=false if the ZIM is
// not mounted (downloading first time, network down, etc.). Never throws.
LookupResult suggest(std::string_view query, int max_results = 5);

// Full-text article search. Same availability semantics.
LookupResult search(std::string_view query, int max_results = 5);

// Render a compact block of top-K suggest()-hit titles + snippets for
// inlining into LLM prompts. Returns "(knowledge offline)" when the ZIM
// is not yet mounted.
std::string render_for_prompt(std::string_view query, int max_results = 3);

// Human-readable one-line status (for stderr / debug):
// "ready (54.2 GiB)" / "downloading 12% (6.4/53.5 GiB, 8 workers)" /
// "offline (no zim, no network)".
std::string status_string();

}
