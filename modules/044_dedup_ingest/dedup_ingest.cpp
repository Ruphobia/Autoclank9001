#include "dedup_ingest.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <sstream>
#include <unordered_map>

namespace dedup_ingest {
namespace {

uint64_t fnv64(std::string_view s) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (unsigned char c : s) { h ^= c; h *= 0x100000001b3ULL; }
    return h;
}

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

bool is_tracking(const std::string & key) {
    static const std::array<const char *, 8> exact = {
        "fbclid","gclid","msclkid","yclid","mc_eid","mc_cid","_ga","_gl"
    };
    for (const auto * e : exact) if (key == e) return true;
    if (key.rfind("utm_", 0) == 0) return true;
    return false;
}

// Minimal URL split: scheme://host[:port]/path?query#frag .
struct Parts {
    std::string scheme, host, port, path, query, frag;
    bool        ok = false;
};

Parts split(std::string_view url) {
    Parts p;
    auto colon = url.find(":");
    if (colon == std::string_view::npos) return p;
    auto rest = url.substr(0, colon);
    p.scheme = lower(rest);
    if (url.size() < colon + 3 || url[colon + 1] != '/' || url[colon + 2] != '/') {
        return p;
    }
    auto after = url.substr(colon + 3);
    // host[:port]
    auto end_host = after.find_first_of("/?#");
    std::string hp;
    if (end_host == std::string_view::npos) {
        hp.assign(after.begin(), after.end());
        after = {};
    } else {
        hp.assign(after.begin(), after.begin() + end_host);
        after.remove_prefix(end_host);
    }
    auto cp = hp.find(':');
    if (cp == std::string::npos) {
        p.host = lower(hp);
    } else {
        p.host = lower(hp.substr(0, cp));
        p.port = hp.substr(cp + 1);
    }
    // path
    if (!after.empty() && after.front() == '/') {
        auto end_path = after.find_first_of("?#");
        if (end_path == std::string_view::npos) {
            p.path.assign(after.begin(), after.end());
            after = {};
        } else {
            p.path.assign(after.begin(), after.begin() + end_path);
            after.remove_prefix(end_path);
        }
    } else {
        p.path = "/";
    }
    if (!after.empty() && after.front() == '?') {
        after.remove_prefix(1);
        auto frag = after.find('#');
        if (frag == std::string_view::npos) { p.query.assign(after.begin(), after.end()); after = {}; }
        else { p.query.assign(after.begin(), after.begin() + frag); after.remove_prefix(frag); }
    }
    if (!after.empty() && after.front() == '#') {
        after.remove_prefix(1);
        p.frag.assign(after.begin(), after.end());
    }
    p.ok = true;
    return p;
}

std::string collapse_slashes(const std::string & in) {
    std::string out;
    out.reserve(in.size());
    bool prev_slash = false;
    for (char c : in) {
        if (c == '/') {
            if (!prev_slash) out.push_back('/');
            prev_slash = true;
        } else {
            out.push_back(c);
            prev_slash = false;
        }
    }
    return out;
}

}

void init()     {}
void shutdown() {}

std::string canonicalize_url(std::string_view url) {
    Parts p = split(url);
    if (!p.ok) return std::string{url};

    // Drop default ports.
    if ((p.scheme == "http"  && p.port == "80") ||
        (p.scheme == "https" && p.port == "443")) {
        p.port.clear();
    }

    // Path: collapse "//", drop trailing "/" when path is not just "/".
    std::string path = collapse_slashes(p.path.empty() ? "/" : p.path);
    if (path.size() > 1 && path.back() == '/') path.pop_back();

    // Query: split, drop tracking params, sort the rest.
    std::vector<std::pair<std::string, std::string>> kvs;
    if (!p.query.empty()) {
        std::string cur_key, cur_val;
        bool in_val = false;
        auto flush = [&]() {
            if (cur_key.empty() && cur_val.empty()) return;
            if (!is_tracking(cur_key)) kvs.emplace_back(cur_key, cur_val);
            cur_key.clear(); cur_val.clear(); in_val = false;
        };
        for (char c : p.query) {
            if (c == '&') { flush(); continue; }
            if (c == '=' && !in_val) { in_val = true; continue; }
            (in_val ? cur_val : cur_key).push_back(c);
        }
        flush();
    }
    std::sort(kvs.begin(), kvs.end());
    std::ostringstream os;
    os << p.scheme << "://" << p.host;
    if (!p.port.empty()) os << ":" << p.port;
    os << path;
    if (!kvs.empty()) {
        os << "?";
        for (size_t i = 0; i < kvs.size(); ++i) {
            if (i) os << "&";
            os << kvs[i].first;
            if (!kvs[i].second.empty()) os << "=" << kvs[i].second;
        }
    }
    // fragment intentionally dropped.
    return os.str();
}

namespace {
std::vector<std::string> tokenize_lower(std::string_view text) {
    std::vector<std::string> out;
    std::string cur;
    auto flush = [&]() {
        if (!cur.empty()) { out.push_back(cur); cur.clear(); }
    };
    for (char c : text) {
        unsigned char uc = (unsigned char)c;
        if (std::isalnum(uc) || c == '_') cur.push_back((char)std::tolower(uc));
        else flush();
    }
    flush();
    return out;
}
}

uint64_t simhash64(std::string_view text) {
    auto toks = tokenize_lower(text);
    if (toks.empty()) return 0;
    std::unordered_map<std::string, int> counts;
    for (auto & t : toks) ++counts[t];

    int acc[64] = {0};
    for (const auto & [tok, w] : counts) {
        uint64_t h = fnv64(tok);
        for (int b = 0; b < 64; ++b) {
            acc[b] += ((h >> b) & 1) ? w : -w;
        }
    }
    uint64_t out = 0;
    for (int b = 0; b < 64; ++b) {
        if (acc[b] > 0) out |= (1ULL << b);
    }
    return out;
}

int hamming_distance(uint64_t a, uint64_t b) {
    uint64_t x = a ^ b;
    int d = 0;
    while (x) { d += (int)(x & 1); x >>= 1; }
    return d;
}

bool is_near_duplicate(std::string_view a, std::string_view b, int threshold) {
    return hamming_distance(simhash64(a), simhash64(b)) <= threshold;
}

}
