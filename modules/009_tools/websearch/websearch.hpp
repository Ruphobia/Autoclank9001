#pragma once

#include <string>
#include <string_view>
#include <vector>

// Outbound reference-lookup tool. Backend: DuckDuckGo's keyless HTML
// endpoint (https://html.duckduckgo.com/html/). Callers are responsible
// for checking project_cfg::web_lookup_enabled() BEFORE invoking anything
// here; this module performs the network I/O unconditionally.
namespace websearch {

struct Hit {
    std::string title;
    std::string url;
    std::string snippet;
};

struct SearchResult {
    bool             ok = false;
    std::string      error;     // populated when ok == false
    std::vector<Hit> hits;
};

// Query DuckDuckGo and return up to `max_hits` organic results.
SearchResult search(std::string_view query, int max_hits = 8);

struct FetchResult {
    bool        ok = false;
    std::string error;
    std::string path;        // where the bytes landed (on ok)
    long        http_code = 0;
    long long   size = 0;
    std::string final_url;   // after redirects
    std::string content_type;
};

// Download `url` to `dest_path` (parent dirs created). Follows redirects,
// sends a browser UA. Does not consult the project flag.
FetchResult fetch_to_file(std::string_view url, std::string_view dest_path);

// Raw GET of a small text/HTML resource into memory (capped). Used for
// snippet grounding of question answers.
struct GetResult {
    bool        ok = false;
    std::string error;
    long        http_code = 0;
    std::string body;
    std::string content_type;
};
GetResult http_get(std::string_view url, long max_bytes = 512 * 1024);

// Render hits as a compact block for injecting into a model prompt / trail.
std::string format_hits(const std::vector<Hit> & hits);

// --- Auto-detection of lookup intent ---

struct Intent {
    bool        needs_lookup = false;
    // true  = the request CANNOT be fulfilled without the network
    //         (obtain a library/asset, post-cutoff fact) — with lookups
    //         disabled the turn must stop and tell the user.
    // false = a lookup would merely enrich the result — with lookups
    //         disabled the pipeline notes that and proceeds locally.
    bool        required = false;
    std::string query;   // the search query to run, when needs_lookup
    std::string reason;  // one short sentence for the trail
};

// Decide whether answering / fulfilling `cleaned` would benefit from an
// outbound reference lookup (fetching a fact that may be post-cutoff, or
// locating an external resource / library to retrieve). `classify_needs_web`
// is the pipeline's existing needs-web tag, used as a prior. Uses the 14B
// model; falls back to a keyword heuristic if the model is unavailable.
Intent detect_intent(std::string_view cleaned, bool classify_needs_web);

// Cheap keyword test: does `cleaned` read like "obtain an external
// artifact" (download / install / integrate a library, font, dataset,
// ...)? Callers use it to corroborate a model 'required' verdict before
// letting a command turn hard-block on a disabled web lookup: a lone
// model misfire must never refuse locally-doable work.
bool obtain_intent(std::string_view cleaned);

// --- Resource-retrieval planning ---

struct DownloadPlan {
    bool                     ok = false;
    std::string              package;    // human name of the chosen library
    std::string              filename;   // suggested local filename
    std::vector<std::string> urls;       // candidate direct-download URLs to try in order
    std::string              api_hint;   // one-line note on the library's CURRENT API
    std::string              note;
};

// Given the original request and the search hits, ask the model for the
// best single library to retrieve and concrete direct-download URLs for
// its standalone/UMD build (CDN links preferred). The caller tries the
// URLs in order and keeps the first that yields a valid asset.
DownloadPlan plan_download(std::string_view request, const std::vector<Hit> & hits);

}  // namespace websearch
