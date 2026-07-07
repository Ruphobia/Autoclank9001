// SPDX-License-Identifier: GPL-3.0-or-later
#include "components.hpp"

#include "../../003_stylize/qwen14b.hpp"

#include <nlohmann/json.hpp>
#include <curl/curl.h>

#include <cctype>
#include <cstdio>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace components {
namespace {

using json = nlohmann::json;

std::string read_credentials_field(const std::string & top, const std::string & key) {
    std::ifstream f("settings/credentials.json");
    if (!f) return {};
    json j = json::parse(f, nullptr, false);
    if (!j.is_object() || !j.contains(top)) return {};
    if (!j[top].is_object() || !j[top].contains(key)) return {};
    return j[top][key].get<std::string>();
}

std::size_t curl_write(char * ptr, std::size_t size, std::size_t nmemb, void * ud) {
    auto * out = static_cast<std::string *>(ud);
    out->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string post_json(const std::string & url, const std::string & body,
                      std::string & err_out) {
    CURL * c = curl_easy_init();
    if (!c) { err_out = "curl init failed"; return {}; }
    std::string buf;
    char err[CURL_ERROR_SIZE] = {0};
    curl_slist * hdrs = nullptr;
    hdrs = curl_slist_append(hdrs, "Content-Type: application/json");
    hdrs = curl_slist_append(hdrs, "Accept: application/json");
    curl_easy_setopt(c, CURLOPT_URL,            url.c_str());
    curl_easy_setopt(c, CURLOPT_POST,           1L);
    curl_easy_setopt(c, CURLOPT_POSTFIELDS,     body.c_str());
    curl_easy_setopt(c, CURLOPT_POSTFIELDSIZE,  static_cast<long>(body.size()));
    curl_easy_setopt(c, CURLOPT_HTTPHEADER,     hdrs);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION,  curl_write);
    curl_easy_setopt(c, CURLOPT_WRITEDATA,      &buf);
    curl_easy_setopt(c, CURLOPT_TIMEOUT,        20L);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_ERRORBUFFER,    err);
    curl_easy_setopt(c, CURLOPT_USERAGENT,      "tool/components");

    CURLcode rc = curl_easy_perform(c);
    long http_code = 0;
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);
    curl_slist_free_all(hdrs);
    curl_easy_cleanup(c);

    if (rc != CURLE_OK) {
        err_out = std::string("curl: ") + (err[0] ? err : curl_easy_strerror(rc));
        return {};
    }
    if (http_code < 200 || http_code >= 300) {
        err_out = "http " + std::to_string(http_code) + ": " + buf;
        return {};
    }
    return buf;
}

// Try to recover a JSON object from arbitrary LLM output: strip code fences
// + commentary, then parse the first `{ ... }` slice.
json parse_loose_json(const std::string & raw) {
    auto first = raw.find('{');
    auto last  = raw.rfind('}');
    if (first == std::string::npos || last == std::string::npos || last <= first)
        return json{};
    std::string slice = raw.substr(first, last - first + 1);
    json j = json::parse(slice, nullptr, false);
    return j;
}

std::string trim(const std::string & s) {
    auto a = s.find_first_not_of(" \t\r\n");
    auto b = s.find_last_not_of(" \t\r\n");
    if (a == std::string::npos) return {};
    return s.substr(a, b - a + 1);
}

// Fix common LLM keyword failure modes: slug-style hyphenation, "3v3"
// shorthand, accidental quoting. Mouser keyword search wants natural
// phrase queries with spaces.
std::string sanitize_keyword(const std::string & kw_in) {
    std::string kw = trim(kw_in);
    // strip surrounding quotes
    if (kw.size() >= 2 &&
        ((kw.front() == '"' && kw.back() == '"') ||
         (kw.front() == '\'' && kw.back() == '\'')))
        kw = kw.substr(1, kw.size() - 2);

    // Slug → words: if no space at all but multiple hyphens, treat as slug.
    if (kw.find(' ') == std::string::npos) {
        int hyphens = 0;
        for (char c : kw) if (c == '-') ++hyphens;
        if (hyphens >= 2) {
            for (char & c : kw) if (c == '-') c = ' ';
        }
    }
    // "3v3" → "3.3V" — Mouser indexes the decimal form.
    {
        static const std::regex re(R"((\d+)v(\d+))",
                                   std::regex::optimize | std::regex::icase);
        kw = std::regex_replace(kw, re, "$1.$2V");
    }
    // Bare "v" suffix on a number → uppercase V ("12v" → "12V"). Only
    // when followed by a non-digit so we don't break "3.3V" we just made.
    {
        static const std::regex re(R"((\d+(?:\.\d+)?)v(?=$|[^A-Za-z0-9]))",
                                   std::regex::optimize | std::regex::icase);
        kw = std::regex_replace(kw, re, "$1V");
    }
    // Collapse whitespace.
    {
        static const std::regex re(R"(\s+)", std::regex::optimize);
        kw = std::regex_replace(kw, re, " ");
    }
    return trim(kw);
}

// Last-ditch retry helper: drop the trailing token to widen the search.
// Returns empty when nothing more can be dropped meaningfully.
std::string relax_keyword(const std::string & kw) {
    auto pos = kw.find_last_of(' ');
    if (pos == std::string::npos) return {};
    std::string out = kw.substr(0, pos);
    // Must still have a component noun (≥ 2 tokens) to be useful.
    if (out.find(' ') == std::string::npos) return {};
    return out;
}

}  // namespace

bool has_credentials() {
    return !read_credentials_field("mouser", "api_key").empty();
}

Intent extract_intent(std::string_view prompt) {
    static constexpr const char * kSystem =
        "You decide what the user wants regarding electronic components.\n"
        "Output STRICT JSON only — no prose, no code fences. Schema:\n"
        "{\n"
        "  \"is_parts_request\":  boolean, // NEW search for a specific component WITH specs\n"
        "  \"use_last_results\":  boolean, // refers to the PRIOR parts result (\"write it to a file\")\n"
        "  \"want_full_list\":    boolean, // ALL results / complete list (\"give me all\")\n"
        "  \"write_to_file\":     boolean, // write/save to a file (md / markdown / file)\n"
        "  \"keyword\":           string,  // Mouser keyword phrase — space-separated, NOT a slug\n"
        "  \"filename\":          string,  // .md filename if write_to_file else \"\"\n"
        "  \"explain\":           string   // one terse sentence on the call\n"
        "}\n"
        "\n"
        "Critical rules for \"keyword\":\n"
        "- Plain English phrase with SPACES between words. NEVER hyphenate the"
        " whole thing into a slug.\n"
        "- Lead with the component noun (\"switching regulator\", \"buck"
        " converter\", \"ceramic capacitor\", \"MOSFET\", …) then the most"
        " specific numeric spec.\n"
        "- Preserve user topology words (\"switching\", \"buck\", \"boost\","
        " \"LDO\", \"linear\") — they drive downstream filtering.\n"
        "- Voltage notation: write \"3.3V\" not \"3v3\"; \"12V\" not \"12v\".\n"
        "- Current: write \"1A\" not \"1amp\".\n"
        "- No verbs, no \"I need\", no quotes inside.\n"
        "\n"
        "EXAMPLES of GOOD keywords:\n"
        "  \"switching regulator 3.3V 1A 12V input\"\n"
        "  \"buck converter 3.3V 1A\"\n"
        "  \"ceramic capacitor 10uF 50V 0805\"\n"
        "  \"voltage regulator 5V 500mA SOT-23\"\n"
        "  \"N-channel MOSFET 60V 30A TO-220\"\n"
        "EXAMPLES of BAD keywords (DO NOT do this):\n"
        "  \"voltage-controller-12v-3v3-1a\"   ← slug, all hyphenated, mangled units\n"
        "  \"I need a 3.3V regulator\"          ← verb, first person\n"
        "  \"3v3\"                               ← shorthand, no component noun\n"
        "  \"buck\"                              ← too short, no specs\n"
        "\n"
        "Other rules:\n"
        "- Conceptual / how-does-it-work questions: all booleans false.\n"
        "- Short follow-ups (\"write it to a file\", \"give me all of those\")"
        " set use_last_results=true and keyword=\"\" (server reuses prior keyword).\n"
        "- is_parts_request and use_last_results are MUTUALLY EXCLUSIVE.\n"
        "- filename: lowercase-with-dashes ending in \".md\" (e.g."
        " \"regulators.md\", \"3v3-bucks.md\"). No paths. \"\" if not saving.\n"
        "- If unsure, set both is_parts_request and use_last_results to false.\n"
        "- Output exactly one JSON object. No markdown.";

    std::string raw = qwen14b::generate(kSystem, prompt, /*max_new_tokens=*/256);
    json j = parse_loose_json(raw);
    Intent out;
    if (j.is_object()) {
        out.is_parts_request = j.value("is_parts_request", false);
        out.use_last_results = j.value("use_last_results", false);
        out.want_full_list   = j.value("want_full_list",   false);
        out.write_to_file    = j.value("write_to_file",    false);
        out.keyword          = trim(j.value("keyword",     std::string{}));
        out.filename         = trim(j.value("filename",    std::string{}));
        out.reasoning        = trim(j.value("explain",     std::string{}));
    } else {
        out.reasoning = "intent JSON parse failed; raw=" + raw.substr(0, 240);
    }
    out.keyword = sanitize_keyword(out.keyword);
    if (out.keyword.empty())            out.is_parts_request = false;
    if (out.is_parts_request)           out.use_last_results = false;
    // Sanity-strip the filename: keep [a-z0-9._-] only and force .md ending.
    if (out.write_to_file) {
        std::string clean;
        for (char c : out.filename) {
            char lc = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if ((lc >= 'a' && lc <= 'z') || (lc >= '0' && lc <= '9') ||
                lc == '.' || lc == '_' || lc == '-') {
                clean.push_back(lc);
            } else if (lc == ' ') {
                clean.push_back('-');
            }
        }
        if (clean.empty()) clean = "parts.md";
        if (clean.size() < 3 || clean.substr(clean.size() - 3) != ".md")
            clean += ".md";
        out.filename = clean;
    } else {
        out.filename.clear();
    }
    return out;
}

std::vector<Part> search(std::string_view keyword_sv, int limit) {
    std::vector<Part> out;
    const std::string key = read_credentials_field("mouser", "api_key");
    if (key.empty()) {
        std::fprintf(stderr, "components: no mouser.api_key in settings/credentials.json\n");
        return out;
    }
    std::string keyword(keyword_sv);
    if (keyword.empty()) return out;
    if (limit <= 0) limit = 10;
    if (limit > 50) limit = 50;

    json req;
    req["SearchByKeywordRequest"]["keyword"]        = keyword;
    req["SearchByKeywordRequest"]["records"]        = limit;
    req["SearchByKeywordRequest"]["startingRecord"] = 0;
    req["SearchByKeywordRequest"]["searchOptions"]  = "InStock";  // we want in-stock by default
    req["SearchByKeywordRequest"]["searchWithYourSignUpLanguage"] = "";

    const std::string url =
        "https://api.mouser.com/api/v1/search/keyword?apiKey=" + key;
    std::string err;
    std::string body = post_json(url, req.dump(), err);
    if (!err.empty()) {
        std::fprintf(stderr, "components: mouser POST failed: %s\n", err.c_str());
        return out;
    }
    json resp = json::parse(body, nullptr, false);
    if (!resp.is_object()) {
        std::fprintf(stderr, "components: mouser response not JSON\n");
        return out;
    }
    if (resp.contains("Errors") && resp["Errors"].is_array() && !resp["Errors"].empty()) {
        std::string msg;
        for (auto & e : resp["Errors"]) {
            if (e.is_object() && e.contains("Message"))
                msg += e["Message"].get<std::string>() + "; ";
        }
        if (!msg.empty())
            std::fprintf(stderr, "components: mouser errors: %s\n", msg.c_str());
    }
    if (!resp.contains("SearchResults")) return out;
    const auto & sr = resp["SearchResults"];
    if (!sr.is_object() || !sr.contains("Parts") || !sr["Parts"].is_array())
        return out;

    // Small helpers used only during Mouser JSON unpacking.
    auto parse_leading_int = [](const std::string & s) -> int {
        int v = 0; bool any = false;
        for (char c : s) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                v = v * 10 + (c - '0'); any = true;
            } else if (c == ',' || c == ' ') {
                continue;
            } else if (any) {
                break;
            } else if (c == '-' || c == '+') {
                continue;
            }
        }
        return any ? v : -1;
    };
    auto parse_weeks = [](const std::string & s) -> int {
        // Accepts "14 Weeks", "6-8 Weeks", "42 Days", "1 Year" etc.
        int n = 0; bool any = false;
        for (char c : s) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                n = n * 10 + (c - '0'); any = true;
            } else if (any) break;
        }
        if (!any) return -1;
        std::string lc; lc.reserve(s.size());
        for (char c : s) lc.push_back(static_cast<char>(
            std::tolower(static_cast<unsigned char>(c))));
        if (lc.find("year")  != std::string::npos) return n * 52;
        if (lc.find("month") != std::string::npos) return n * 4;
        if (lc.find("day")   != std::string::npos)
            return n < 7 ? 1 : n / 7;
        return n;   // default to weeks
    };
    auto truthy = [](const json & v) -> bool {
        if (v.is_boolean()) return v.get<bool>();
        if (v.is_number())  return v.get<double>() != 0.0;
        if (v.is_string()) {
            std::string s = v.get<std::string>();
            for (auto & c : s) c = static_cast<char>(
                std::tolower(static_cast<unsigned char>(c)));
            return s == "true" || s == "yes" || s == "y" || s == "1";
        }
        return false;
    };

    for (const auto & p : sr["Parts"]) {
        if (!p.is_object()) continue;
        Part pt;
        pt.mfg_part_no    = p.value("ManufacturerPartNumber", std::string{});
        pt.mfg            = p.value("Manufacturer",           std::string{});
        pt.desc           = p.value("Description",            std::string{});
        pt.mouser_part_no = p.value("MouserPartNumber",       std::string{});
        pt.datasheet_url  = p.value("DataSheetUrl",           std::string{});
        pt.product_url    = p.value("ProductDetailUrl",       std::string{});
        pt.availability   = p.value("AvailabilityInStock",    std::string{});
        pt.category       = p.value("Category",               std::string{});

        // Health fields — Mouser returns them all in the same object.
        pt.in_stock   = parse_leading_int(pt.availability);
        pt.lead_time  = p.value("LeadTime", std::string{});
        pt.lead_time_weeks = pt.lead_time.empty() ? -1 : parse_weeks(pt.lead_time);
        // Lifecycle can come as LifecycleStatus (Mouser core) OR
        // ProductStatus (some suppliers). Prefer whichever is present.
        {
            std::string ls = p.value("LifecycleStatus", std::string{});
            if (ls.empty()) ls = p.value("ProductStatus", std::string{});
            pt.lifecycle = ls;
        }
        if (p.contains("IsObsolete"))            pt.is_obsolete   = truthy(p["IsObsolete"]);
        if (p.contains("NonCancelableNonReturnable"))
            pt.ncnr = truthy(p["NonCancelableNonReturnable"]);
        pt.rohs_status     = p.value("ROHSStatus",           std::string{});
        pt.restriction_msg = p.value("RestrictionMessage",   std::string{});
        pt.suggested_replacement = p.value("SuggestedReplacement", std::string{});
        {
            std::string mn = p.value("Min",  std::string{"1"});
            std::string ml = p.value("Mult", std::string{"1"});
            int v; v = parse_leading_int(mn); if (v > 0) pt.min_qty  = v;
                    v = parse_leading_int(ml); if (v > 0) pt.qty_mult = v;
        }
        // On-order aggregate: sum every AvailabilityOnOrder[].Quantity.
        if (p.contains("AvailabilityOnOrder") &&
            p["AvailabilityOnOrder"].is_array())
        {
            int sum = 0;
            for (const auto & oo : p["AvailabilityOnOrder"]) {
                if (!oo.is_object()) continue;
                if (oo.contains("Quantity")) {
                    if (oo["Quantity"].is_number()) sum += oo["Quantity"].get<int>();
                    else if (oo["Quantity"].is_string()) {
                        int v = parse_leading_int(oo["Quantity"].get<std::string>());
                        if (v > 0) sum += v;
                    }
                }
            }
            pt.on_order = sum;
        }
        // Full price ladder.
        if (p.contains("PriceBreaks") && p["PriceBreaks"].is_array() &&
            !p["PriceBreaks"].empty())
        {
            for (const auto & pb : p["PriceBreaks"]) {
                if (!pb.is_object()) continue;
                int qty = 1;
                if (pb.contains("Quantity")) {
                    if (pb["Quantity"].is_number()) qty = pb["Quantity"].get<int>();
                    else if (pb["Quantity"].is_string())
                        qty = parse_leading_int(pb["Quantity"].get<std::string>());
                }
                double price = 0.0;
                if (pb.contains("Price")) {
                    if (pb["Price"].is_number()) price = pb["Price"].get<double>();
                    else if (pb["Price"].is_string()) {
                        // "$1.234" or "€2,34" — strip non-digits/dot.
                        std::string s = pb["Price"].get<std::string>();
                        std::string cleaned;
                        for (char c : s) {
                            if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') cleaned += c;
                        }
                        if (!cleaned.empty()) {
                            try { price = std::stod(cleaned); } catch (...) { price = 0.0; }
                        }
                    }
                }
                pt.price_breaks.emplace_back(qty > 0 ? qty : 1, price);
            }
            if (!pt.price_breaks.empty() && pt.price_at_1.empty()) {
                char buf[32];
                std::snprintf(buf, sizeof(buf), "$%.4f", pt.price_breaks.front().second);
                pt.price_at_1 = buf;
            }
        }
        // Run the rule set BEFORE moving into the vector so downstream
        // consumers always see the annotated part.
        analyze_health(pt);
        out.push_back(std::move(pt));
    }

    // Topology filter: Mouser's keyword search ranks loosely; "switching
    // regulator" returns LDOs near the top because they match on
    // "regulator". Post-filter so user-stated topology actually sticks.
    auto lower = [](std::string s) {
        for (auto & c : s) c = static_cast<char>(
            std::tolower(static_cast<unsigned char>(c)));
        return s;
    };
    std::string kw_lc = lower(keyword);
    const bool want_switching =
        kw_lc.find("switch")  != std::string::npos ||
        kw_lc.find("buck")    != std::string::npos ||
        kw_lc.find("boost")   != std::string::npos ||
        kw_lc.find("smps")    != std::string::npos;
    const bool want_linear =
        !want_switching && (
            kw_lc.find("ldo")    != std::string::npos ||
            kw_lc.find("linear") != std::string::npos);
    if (want_switching || want_linear) {
        auto contains_token = [](const std::string & hay,
                                 const std::string & needle) {
            auto pos = hay.find(needle);
            if (pos == std::string::npos) return false;
            // ensure it's a whole token (avoid hits like "underline" for "line")
            const auto before = pos == 0 ? ' ' : hay[pos - 1];
            const auto after  = pos + needle.size() >= hay.size()
                                ? ' ' : hay[pos + needle.size()];
            auto edge = [](char c){
                return !std::isalpha(static_cast<unsigned char>(c));
            };
            return edge(before) && edge(after);
        };
        std::vector<Part> kept;
        kept.reserve(out.size());
        for (auto & p : out) {
            std::string d = lower(p.desc) + " " + lower(p.category);
            bool drop = false;
            if (want_switching) {
                if (contains_token(d, "ldo") || contains_token(d, "linear"))
                    drop = true;
            } else if (want_linear) {
                if (contains_token(d, "switching") ||
                    contains_token(d, "buck")     ||
                    contains_token(d, "boost"))
                    drop = true;
            }
            if (!drop) kept.push_back(std::move(p));
        }
        out = std::move(kept);
    }
    return out;
}

std::vector<Part> search_with_retry(std::string_view keyword,
                                    std::string & final_keyword,
                                    int limit, int max_retries) {
    std::string kw(keyword);
    for (int i = 0; i <= max_retries; ++i) {
        auto parts = search(kw, limit);
        if (!parts.empty()) { final_keyword = kw; return parts; }
        std::string next = relax_keyword(kw);
        if (next.empty() || next == kw) break;
        std::fprintf(stderr, "components: 0 hits for \"%s\"; retrying with "
                             "\"%s\"\n", kw.c_str(), next.c_str());
        kw = next;
    }
    final_keyword = kw;
    return {};
}

std::string format_results(const std::vector<Part> & parts,
                           std::string_view keyword,
                           int max_shown) {
    if (parts.empty()) {
        return std::string("No Mouser hits for `") + std::string(keyword) +
               "`. Try widening the keyword (drop a spec or two) or check the "
               "Mouser API key in Settings → API Credentials…";
    }
    if (max_shown <= 0) max_shown = 5;

    // Sort: critical-flagged parts to the top (the user needs to see a
    // dead SKU before a price), then by cheapest in-stock. Parse "$x.yz"
    // loosely.
    std::vector<std::size_t> order(parts.size());
    for (std::size_t i = 0; i < parts.size(); ++i) order[i] = i;
    auto price_key = [&](std::size_t i) -> double {
        const std::string & s = parts[i].price_at_1;
        double v = 1e9;          // missing-price → push to bottom
        std::string n;
        for (char c : s) if ((c >= '0' && c <= '9') || c == '.') n.push_back(c);
        try { if (!n.empty()) v = std::stod(n); } catch (...) {}
        return v;
    };
    auto severity_rank = [&](std::size_t i) -> int {
        const std::string sev = worst_severity(parts[i]);
        if (sev == "critical") return 3;
        if (sev == "warn")     return 2;
        if (sev == "info")     return 1;
        return 0;
    };
    std::sort(order.begin(), order.end(),
        [&](std::size_t a, std::size_t b) {
            const int sa = severity_rank(a), sb = severity_rank(b);
            if (sa != sb) return sa > sb;
            return price_key(a) < price_key(b);
        });

    std::ostringstream out;
    out << "# Mouser results for `" << keyword << "`\n";
    out << "_Sorted by anomaly severity then lowest unit price._\n\n";
    int shown = 0;
    for (std::size_t idx : order) {
        if (shown >= max_shown) break;
        const Part & p = parts[idx];
        out << (shown + 1) << ". **" << p.mfg_part_no << "** — " << p.mfg << "\n";
        if (!p.desc.empty()) out << "   " << p.desc << "\n";
        out << "   ";
        if (!p.price_at_1.empty())  out << p.price_at_1 << " @ qty 1";
        else                        out << "(no price)";
        if (!p.availability.empty()) out << " — " << p.availability << " in stock";
        out << "\n";
        if (!p.datasheet_url.empty() || !p.product_url.empty()) {
            out << "   ";
            if (!p.datasheet_url.empty())
                out << "[datasheet](" << p.datasheet_url << ")";
            if (!p.datasheet_url.empty() && !p.product_url.empty())
                out << " · ";
            if (!p.product_url.empty())
                out << "[mouser](" << p.product_url << ")";
            out << "\n";
        }
        // Flags: rendered as a nested bullet block under the part so the
        // user sees WHY a part sorted to the top when it did.
        if (!p.flags.empty()) {
            out << "   ⚑ Flags:\n";
            for (const auto & f : p.flags) {
                out << "     - **" << f.code << "** (`" << f.severity
                    << "`, Mouser " << f.field << "): " << f.message << "\n";
            }
        }
        out << "\n";
        ++shown;
    }
    if (static_cast<int>(parts.size()) > shown) {
        out << "_+ " << (parts.size() - shown) << " more — ask \"give me all"
            << " the results\" or \"write them to a file\" for the full list._\n";
    }
    return out.str();
}

// ---- Health analysis ------------------------------------------------------
//
// 12 rules, each with a stable code + severity + user-visible message +
// Mouser source field(s). Applied to every Part at the tail of
// search(); idempotent by (code) so re-analyzing a Part is safe.
//
// Severity is a hint for UI (badge color) and for sort — critical
// sorts to the top so a dead SKU wins the eye over a cheap one.

std::string worst_severity(const Part & p) {
    int rank = 0;
    std::string sev;
    for (const auto & f : p.flags) {
        int r = 0;
        if (f.severity == "critical") r = 3;
        else if (f.severity == "warn") r = 2;
        else if (f.severity == "info") r = 1;
        if (r > rank) { rank = r; sev = f.severity; }
    }
    return sev;
}

void analyze_health(Part & p) {
    // Idempotent: only push a flag when its code is not already present.
    auto has_flag = [&](const std::string & code) {
        for (const auto & f : p.flags) if (f.code == code) return true;
        return false;
    };
    auto push = [&](const char * code, const char * sev,
                    const char * msg, const char * field) {
        if (has_flag(code)) return;
        p.flags.push_back({code, sev, msg, field});
    };

    // --- Inventory ---
    // OUT_OF_STOCK: zero on shelf.
    if (p.in_stock == 0) {
        push("OUT_OF_STOCK", "critical",
             "Zero units on the shelf — order will back-order or fail.",
             "AvailabilityInStock");
    }
    // LOW_STOCK: <10.
    else if (p.in_stock > 0 && p.in_stock < 10) {
        push("LOW_STOCK", "warn",
             "Fewer than 10 units left; risk of exhaustion mid-project.",
             "AvailabilityInStock");
    }
    // THIN_STOCK: 10..99.
    else if (p.in_stock >= 10 && p.in_stock < 100) {
        push("THIN_STOCK", "info",
             "Only tens available; caution for anything beyond a prototype build.",
             "AvailabilityInStock");
    }

    // STOCK_GAP: nothing on shelf, nothing on order, factory lead only.
    if (p.in_stock == 0 && p.on_order == 0 && !p.lead_time.empty()) {
        push("STOCK_GAP", "critical",
             "Nothing on shelf, nothing on order, factory lead only — order today.",
             "AvailabilityInStock+AvailabilityOnOrder+LeadTime");
    }

    // --- Lifecycle ---
    std::string ll; ll.reserve(p.lifecycle.size());
    for (char c : p.lifecycle) ll.push_back(static_cast<char>(
        std::tolower(static_cast<unsigned char>(c))));
    const bool eol =
        p.is_obsolete ||
        ll.find("eol")          != std::string::npos ||
        ll.find("end of life")  != std::string::npos ||
        ll.find("discontinued") != std::string::npos ||
        ll.find("obsolete")     != std::string::npos;
    const bool nrnd =
        ll.find("nrnd") != std::string::npos ||
        ll.find("not recommended") != std::string::npos ||
        ll.find("not for new")     != std::string::npos;
    if (eol) {
        push("EOL", "critical",
             "End-of-life — remaining inventory is what exists; find a replacement.",
             "LifecycleStatus / IsObsolete");
    } else if (nrnd) {
        push("LIFECYCLE_RISK", "warn",
             "Manufacturer flagged as not-recommended for new design.",
             "LifecycleStatus");
    }

    // --- Lead time ---
    if (p.lead_time_weeks > 26) {
        push("LONG_LEAD", "warn",
             "Factory lead time >6 months; block on procurement, not on design.",
             "LeadTime");
    }

    // --- Procurement / order rules ---
    if (p.ncnr) {
        push("NCNR", "warn",
             "Non-cancelable / non-returnable — order commits capital irrevocably.",
             "NonCancelableNonReturnable");
    }
    if (p.min_qty >= 100 && !p.price_at_1.empty()) {
        push("HIGH_MOQ", "info",
             "Minimum order quantity >=100; hobby / prototype quantities may not ship.",
             "Min");
    }
    if (!p.restriction_msg.empty()) {
        push("RESTRICTED", "warn",
             "Export / shipping / access restricted — verify destination and end-use.",
             "RestrictionMessage");
    }

    // --- Compliance ---
    std::string rr; rr.reserve(p.rohs_status.size());
    for (char c : p.rohs_status) rr.push_back(static_cast<char>(
        std::tolower(static_cast<unsigned char>(c))));
    if (rr.find("not compliant") != std::string::npos ||
        rr.find("contains lead") != std::string::npos) {
        push("NON_ROHS", "info",
             "Not RoHS-compliant — reject for EU / consumer product BOMs.",
             "ROHSStatus");
    }

    // --- Price ladder ---
    if (p.price_breaks.size() >= 2) {
        double qty1 = p.price_breaks.front().second;
        double top  = p.price_breaks.back().second;
        // Ratio guard: only when qty1 > 0 and volume break is markedly
        // cheaper (>=6.7x lower). Suggests distributor markup.
        if (qty1 > 0.0 && top > 0.0 && (top / qty1) < 0.15) {
            push("PRICE_JUMP", "info",
                 "qty-1 vs. qty-max break spread >6.7x — likely distributor markup, "
                 "source direct at volume.",
                 "PriceBreaks");
        }
    }
}

}  // namespace components
