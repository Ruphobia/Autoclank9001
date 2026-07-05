// SPDX-License-Identifier: GPL-3.0-or-later
#include "websearch.hpp"

#include <curl/curl.h>

#include <array>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <regex>
#include <string>

namespace fs = std::filesystem;

namespace websearch {
namespace {

constexpr const char * kUA =
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
    "(KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36";

std::size_t write_to_string(char * ptr, std::size_t sz, std::size_t nm, void * ud) {
    auto * s = static_cast<std::string *>(ud);
    const std::size_t n = sz * nm;
    // Cap growth so a huge page can't blow up memory.
    if (s->size() + n > 8u * 1024 * 1024) return 0;
    s->append(ptr, n);
    return n;
}

std::string url_encode(std::string_view s) {
    std::string out;
    out.reserve(s.size() * 3);
    for (unsigned char c : s) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out.push_back(static_cast<char>(c));
        } else if (c == ' ') {
            out.push_back('+');
        } else {
            char buf[4];
            std::snprintf(buf, sizeof(buf), "%%%02X", c);
            out.append(buf);
        }
    }
    return out;
}

std::string url_decode(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            auto hex = [](char c) -> int {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                return -1;
            };
            const int hi = hex(s[i + 1]), lo = hex(s[i + 2]);
            if (hi >= 0 && lo >= 0) {
                out.push_back(static_cast<char>((hi << 4) | lo));
                i += 2;
                continue;
            }
        }
        if (s[i] == '+') out.push_back(' ');
        else             out.push_back(s[i]);
    }
    return out;
}

std::string html_unescape(std::string s) {
    struct Ent { const char * name; const char * rep; };
    static const Ent ents[] = {
        {"&amp;", "&"}, {"&lt;", "<"}, {"&gt;", ">"},
        {"&quot;", "\""}, {"&#39;", "'"}, {"&#x27;", "'"},
        {"&#x2F;", "/"}, {"&nbsp;", " "},
    };
    for (const auto & e : ents) {
        std::string::size_type p = 0;
        const std::string from = e.name;
        while ((p = s.find(from, p)) != std::string::npos) {
            s.replace(p, from.size(), e.rep);
            p += std::strlen(e.rep);
        }
    }
    return s;
}

std::string strip_tags(const std::string & html) {
    std::string out;
    out.reserve(html.size());
    bool in_tag = false;
    for (char c : html) {
        if (c == '<') in_tag = true;
        else if (c == '>') in_tag = false;
        else if (!in_tag) out.push_back(c);
    }
    return html_unescape(out);
}

std::string trim(const std::string & s) {
    std::size_t b = 0, e = s.size();
    auto ws = [](char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; };
    while (b < e && ws(s[b])) ++b;
    while (e > b && ws(s[e - 1])) --e;
    return s.substr(b, e - b);
}

// Turn a DDG redirect href (//duckduckgo.com/l/?uddg=<enc>&...) into the
// real destination URL. Direct hrefs pass through unchanged.
std::string resolve_ddg_href(std::string href) {
    const auto pos = href.find("uddg=");
    if (pos == std::string::npos) {
        if (href.rfind("//", 0) == 0) href = "https:" + href;
        return href;
    }
    std::string enc = href.substr(pos + 5);
    const auto amp = enc.find('&');
    if (amp != std::string::npos) enc.resize(amp);
    return url_decode(enc);
}

std::string curl_fetch(const std::string & url, bool post_body_search,
                       const std::string & post_body, std::string & out_body,
                       long & http_code, std::string & err) {
    CURL * c = curl_easy_init();
    if (!c) { err = "curl init failed"; return {}; }
    char errbuf[CURL_ERROR_SIZE] = {0};
    curl_easy_setopt(c, CURLOPT_URL, url.c_str());
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_to_string);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &out_body);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(c, CURLOPT_USERAGENT, kUA);
    curl_easy_setopt(c, CURLOPT_ACCEPT_ENCODING, "");   // let curl decompress
    curl_easy_setopt(c, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(c, CURLOPT_TIMEOUT, 25L);
    curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 12L);
    struct curl_slist * hdrs = nullptr;
    hdrs = curl_slist_append(hdrs, "Accept-Language: en-US,en;q=0.9");
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, hdrs);
    if (post_body_search) {
        curl_easy_setopt(c, CURLOPT_POSTFIELDS, post_body.c_str());
    }
    const CURLcode rc = curl_easy_perform(c);
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);
    if (hdrs) curl_slist_free_all(hdrs);
    curl_easy_cleanup(c);
    if (rc != CURLE_OK) {
        err = std::string("curl: ") + (errbuf[0] ? errbuf : curl_easy_strerror(rc));
        return {};
    }
    return out_body;
}

// Parse the classic html.duckduckgo.com/html/ layout.
std::vector<Hit> parse_html_layout(const std::string & body, int max_hits) {
    std::vector<Hit> hits;
    // result__a anchors carry url + title; result__snippet holds the text.
    static const std::regex a_re(
        R"rx(class="result__a"[^>]*href="([^"]+)"[^>]*>(.*?)</a>)rx",
        std::regex::icase);
    static const std::regex snip_re(
        R"rx(class="result__snippet"[^>]*>(.*?)</a>)rx", std::regex::icase);

    std::vector<std::string> snippets;
    for (auto it = std::sregex_iterator(body.begin(), body.end(), snip_re);
         it != std::sregex_iterator(); ++it) {
        snippets.push_back(strip_tags((*it)[1].str()));
    }
    std::size_t idx = 0;
    for (auto it = std::sregex_iterator(body.begin(), body.end(), a_re);
         it != std::sregex_iterator() && static_cast<int>(hits.size()) < max_hits;
         ++it, ++idx) {
        Hit h;
        h.url   = resolve_ddg_href((*it)[1].str());
        h.title = trim(strip_tags((*it)[2].str()));
        if (idx < snippets.size()) h.snippet = trim(snippets[idx]);
        if (!h.url.empty() && h.url.rfind("http", 0) == 0) hits.push_back(std::move(h));
    }
    return hits;
}

// Parse the lite.duckduckgo.com/lite/ layout (simpler, scraping-tolerant).
std::vector<Hit> parse_lite_layout(const std::string & body, int max_hits) {
    std::vector<Hit> hits;
    static const std::regex a_re(
        R"rx(<a[^>]+class="result-link"[^>]*href="([^"]+)"[^>]*>(.*?)</a>)rx",
        std::regex::icase);
    static const std::regex snip_re(
        R"rx(<td[^>]*class="result-snippet"[^>]*>(.*?)</td>)rx", std::regex::icase);

    std::vector<std::string> snippets;
    for (auto it = std::sregex_iterator(body.begin(), body.end(), snip_re);
         it != std::sregex_iterator(); ++it) {
        snippets.push_back(strip_tags((*it)[1].str()));
    }
    std::size_t idx = 0;
    for (auto it = std::sregex_iterator(body.begin(), body.end(), a_re);
         it != std::sregex_iterator() && static_cast<int>(hits.size()) < max_hits;
         ++it, ++idx) {
        Hit h;
        h.url   = resolve_ddg_href((*it)[1].str());
        h.title = trim(strip_tags((*it)[2].str()));
        if (idx < snippets.size()) h.snippet = trim(snippets[idx]);
        if (!h.url.empty() && h.url.rfind("http", 0) == 0) hits.push_back(std::move(h));
    }
    return hits;
}

}  // namespace

SearchResult search(std::string_view query, int max_hits) {
    SearchResult r;
    const std::string enc = url_encode(query);

    // Primary: classic HTML endpoint (best organic results).
    {
        std::string body, err;
        long code = 0;
        const std::string url = "https://html.duckduckgo.com/html/?q=" + enc;
        curl_fetch(url, /*post*/ false, {}, body, code, err);
        if (!body.empty()) {
            r.hits = parse_html_layout(body, max_hits);
            if (!r.hits.empty()) { r.ok = true; return r; }
        }
        if (r.error.empty() && !err.empty()) r.error = err;
    }

    // Fallback: lite endpoint (POST), simpler markup, more scraping-tolerant.
    {
        std::string body, err;
        long code = 0;
        curl_fetch("https://lite.duckduckgo.com/lite/", /*post*/ true,
                   "q=" + enc, body, code, err);
        if (!body.empty()) {
            r.hits = parse_lite_layout(body, max_hits);
            if (r.hits.empty()) r.hits = parse_html_layout(body, max_hits);
            if (!r.hits.empty()) { r.ok = true; return r; }
        }
        if (r.error.empty() && !err.empty()) r.error = err;
    }

    if (r.error.empty()) r.error = "no results parsed from DuckDuckGo";
    return r;
}

FetchResult fetch_to_file(std::string_view url, std::string_view dest_path) {
    FetchResult r;
    const fs::path dest(std::string{dest_path});
    std::error_code ec;
    if (dest.has_parent_path()) fs::create_directories(dest.parent_path(), ec);

    FILE * f = std::fopen(dest.string().c_str(), "wb");
    if (!f) { r.error = "cannot open " + dest.string(); return r; }

    CURL * c = curl_easy_init();
    if (!c) { std::fclose(f); r.error = "curl init failed"; return r; }
    char errbuf[CURL_ERROR_SIZE] = {0};
    char ctbuf[256] = {0};
    curl_easy_setopt(c, CURLOPT_URL, std::string(url).c_str());
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION,
        +[](char * p, std::size_t s, std::size_t n, void * ud) -> std::size_t {
            return std::fwrite(p, s, n, static_cast<FILE *>(ud));
        });
    curl_easy_setopt(c, CURLOPT_WRITEDATA, f);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_MAXREDIRS, 8L);
    curl_easy_setopt(c, CURLOPT_USERAGENT, kUA);
    curl_easy_setopt(c, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(c, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(c, CURLOPT_TIMEOUT, 120L);
    curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 20L);

    const CURLcode rc = curl_easy_perform(c);
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &r.http_code);
    char * eff = nullptr;
    curl_easy_getinfo(c, CURLINFO_EFFECTIVE_URL, &eff);
    if (eff) r.final_url = eff;
    char * ct = nullptr;
    curl_easy_getinfo(c, CURLINFO_CONTENT_TYPE, &ct);
    if (ct) r.content_type = ct;
    (void)ctbuf;
    curl_easy_cleanup(c);
    std::fclose(f);

    if (rc != CURLE_OK) {
        fs::remove(dest, ec);
        r.error = std::string("curl: ") + (errbuf[0] ? errbuf : curl_easy_strerror(rc));
        return r;
    }
    if (r.http_code < 200 || r.http_code >= 300) {
        fs::remove(dest, ec);
        r.error = "http " + std::to_string(r.http_code);
        return r;
    }
    r.size = static_cast<long long>(fs::file_size(dest, ec));
    r.ok = true;
    r.path = dest.string();
    return r;
}

GetResult http_get(std::string_view url, long max_bytes) {
    (void)max_bytes;  // write_to_string enforces its own hard cap
    GetResult r;
    std::string body, err;
    long code = 0;
    curl_fetch(std::string(url), false, {}, body, code, err);
    r.http_code = code;
    if (!err.empty() && body.empty()) { r.error = err; return r; }
    if (code < 200 || code >= 300) { r.error = "http " + std::to_string(code); return r; }
    r.ok = true;
    r.body = std::move(body);
    return r;
}

std::string format_hits(const std::vector<Hit> & hits) {
    std::string out;
    for (std::size_t i = 0; i < hits.size(); ++i) {
        out += std::to_string(i + 1) + ". " + hits[i].title + "\n";
        out += "   " + hits[i].url + "\n";
        if (!hits[i].snippet.empty()) out += "   " + hits[i].snippet + "\n";
    }
    if (out.empty()) out = "(no results)";
    return out;
}

}  // namespace websearch
