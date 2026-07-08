// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>
#include <vector>

// Mouser-backed electronic-component lookup. Used by the chat pipeline when
// the user is asking to FIND a specific part (e.g. "I need a 3.3V SMD
// regulator, 1A, switching, low cost") instead of asking a general
// electronics question. The Mouser API key lives in settings/credentials.json
// under `mouser.api_key` - populated via the web UI (Settings → API
// Credentials…). When the key is missing we degrade gracefully (the chat
// pipeline falls through to the regular electronics LLM answer).
namespace components {

// One anomaly detected against a Part. Populated by analyze_health().
// Emitted verbatim on the SSE `component_health` layer frame and folded
// into the answer as a callout so the user sees the anomaly before the
// literal answer to "how many are in stock?".
struct Flag {
    std::string code;       // stable identifier, e.g. "EOL", "LOW_STOCK"
    std::string severity;   // "info" | "warn" | "critical"
    std::string message;    // one-sentence user-visible rationale
    std::string field;      // Mouser field name(s) that triggered it
};

struct Part {
    std::string mfg_part_no;
    std::string mfg;
    std::string desc;
    std::string mouser_part_no;
    std::string datasheet_url;
    std::string product_url;
    std::string availability;     // human-readable stock string from Mouser
    std::string price_at_1;       // first price-break ("$2.34")
    std::string category;

    // Health-analysis fields - parsed alongside the existing fields
    // inside search() so analyze_health() can rule on them without
    // re-fetching. -1 means unknown / unparseable.
    int         in_stock       = -1;
    int         on_order       = 0;
    std::string lead_time;                     // raw "14 Weeks" style
    int         lead_time_weeks = -1;          // parsed; -1 if unknown
    std::string lifecycle;                     // LifecycleStatus / ProductStatus
    bool        is_obsolete    = false;
    bool        ncnr           = false;
    std::string rohs_status;
    std::string restriction_msg;
    std::string suggested_replacement;
    int         min_qty        = 1;
    int         qty_mult       = 1;
    // Full price ladder: (min_qty, unit_price). price_at_1 mirrors [0].
    std::vector<std::pair<int, double>> price_breaks;

    // Populated by analyze_health(). Empty when the part looked clean.
    std::vector<Flag> flags;
};

struct Intent {
    bool        is_parts_request = false;
    std::string keyword;          // search query to send to Mouser
    std::string reasoning;        // short LLM-emitted rationale (for the layer trail)
    bool        write_to_file    = false;
    std::string filename;         // suggested .md filename when write_to_file
    bool        use_last_results = false; // "save the previous list", "write it to a file"
    bool        want_full_list   = false; // "give me all the results", "show everything"
};

// True if `settings/credentials.json` has a non-empty `mouser.api_key`.
bool has_credentials();

// Use the shared 14B model to decide whether the prompt is a parts-search
// request, and if so produce a Mouser-ready keyword. Non-parts prompts
// return is_parts_request=false (and the chat handler falls back to the
// generic electronics LLM answer).
Intent extract_intent(std::string_view prompt);

// POST https://api.mouser.com/api/v1/search/keyword?apiKey=...
// with the given keyword. Empty result vector on HTTP / parse error
// (logs to stderr). Returns at most `limit` parts; 0 means library default
// (Mouser caps individual responses at 50).
std::vector<Part> search(std::string_view keyword, int limit = 10);

// Search with progressive relaxation: if the initial keyword returns 0
// hits, drop the trailing token and retry, up to `max_retries` times.
// Returns the parts found AND, via `final_keyword`, the keyword that
// actually produced them (may be shorter than the input).
std::vector<Part> search_with_retry(std::string_view keyword,
                                    std::string & final_keyword,
                                    int limit = 20,
                                    int max_retries = 3);

// Render the result set as a chat-friendly markdown block. Empty / single-
// item summaries handled inside. `keyword` is included so the user sees
// what Mouser was actually queried for. `max_shown` caps the rendered
// row count; pass a number larger than parts.size() to include all.
std::string format_results(const std::vector<Part> & parts,
                           std::string_view keyword,
                           int max_shown = 5);

// Run the health rule set (OUT_OF_STOCK, LOW_STOCK, LIFECYCLE_RISK,
// EOL, LONG_LEAD, STOCK_GAP, NCNR, HIGH_MOQ, RESTRICTED, NON_ROHS,
// PRICE_JUMP, THIN_STOCK) against a single Part and push every
// triggered flag into p.flags. Idempotent - calling twice de-duplicates
// on `code`. Called automatically from the tail of search(); exposed
// here so tests and future callers (BOM auditors, batch scans) can
// re-analyze without re-fetching.
void analyze_health(Part & p);

// Highest severity found in p.flags. Returns "critical" > "warn" >
// "info" > "" (none). Used by format_results and the SSE frame to
// short-cut a per-part rank without walking the whole list.
std::string worst_severity(const Part & p);

}
