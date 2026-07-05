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
        if (p.contains("PriceBreaks") && p["PriceBreaks"].is_array() &&
            !p["PriceBreaks"].empty())
        {
            const auto & pb = p["PriceBreaks"][0];
            if (pb.is_object() && pb.contains("Price"))
                pt.price_at_1 = pb["Price"].get<std::string>();
        }
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

    // Sort: cheapest in-stock first. Parse "$x.yz" loosely.
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
    std::sort(order.begin(), order.end(),
        [&](std::size_t a, std::size_t b) { return price_key(a) < price_key(b); });

    std::ostringstream out;
    out << "# Mouser results for `" << keyword << "`\n";
    out << "_Sorted by lowest unit price, in-stock only._\n\n";
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
        out << "\n";
        ++shown;
    }
    if (static_cast<int>(parts.size()) > shown) {
        out << "_+ " << (parts.size() - shown) << " more — ask \"give me all"
            << " the results\" or \"write them to a file\" for the full list._\n";
    }
    return out.str();
}

}  // namespace components
