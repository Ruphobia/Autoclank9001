#include "page_triage.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace page_triage {
namespace {

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

std::vector<std::string> tokenize(std::string_view s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        unsigned char uc = (unsigned char)c;
        if (std::isalpha(uc)) cur.push_back((char)std::tolower(uc));
        else if (!cur.empty()) { out.push_back(cur); cur.clear(); }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

struct LangProfile {
    const char *               code;
    std::unordered_set<std::string> words;
};

const std::vector<LangProfile> & profiles() {
    static const std::vector<LangProfile> p = {
        {"en", {"the","of","and","to","a","in","is","that","for","it","on","with","as","this","by"}},
        {"es", {"el","la","de","que","y","en","un","es","los","una","para","con","del","las","se"}},
        {"fr", {"le","la","de","et","les","des","un","une","est","pour","dans","que","au","du","en"}},
        {"de", {"der","die","und","den","ein","ist","das","nicht","mit","sich","auch","von","im","des","auf"}},
    };
    return p;
}

}

void init()     {}
void shutdown() {}

std::string detect_language(std::string_view text) {
    auto toks = tokenize(text);
    if (toks.empty()) return "?";
    std::unordered_map<std::string, int> bag;
    for (auto & t : toks) ++bag[t];

    std::string best = "?";
    int         best_score = 0;
    for (const auto & p : profiles()) {
        int s = 0;
        for (const auto & w : p.words) {
            auto it = bag.find(w);
            if (it != bag.end()) s += it->second;
        }
        if (s > best_score) { best_score = s; best = p.code; }
    }
    if (best_score < 3) return "?";
    return best;
}

double estimate_quality(std::string_view text, std::string_view html) {
    if (text.empty()) return 0.0;
    auto toks = tokenize(text);
    size_t tok_count = toks.size();
    if (tok_count < 8) return 0.05;

    // Sentence count by terminator.
    int sentences = 0;
    for (char c : text) if (c == '.' || c == '!' || c == '?') ++sentences;
    if (sentences < 1) sentences = 1;

    // Alpha ratio over non-whitespace.
    size_t alpha = 0, non_ws = 0;
    for (char c : text) {
        if (std::isspace(static_cast<unsigned char>(c))) continue;
        ++non_ws;
        if (std::isalpha(static_cast<unsigned char>(c))) ++alpha;
    }
    double alpha_ratio = non_ws ? (double)alpha / (double)non_ws : 0.0;

    double length_score = std::min(1.0, (double)tok_count / 200.0);
    double sent_score   = std::min(1.0, (double)sentences  / 5.0);

    // Boilerplate / ad penalty.
    std::string l = lower(text);
    int boiler_hits = 0;
    for (const char * w : {"subscribe", "newsletter", "advertisement",
                            "cookie", "sign in to continue", "404"}) {
        if (l.find(w) != std::string::npos) ++boiler_hits;
    }
    double boiler_penalty = std::min(0.5, boiler_hits * 0.1);

    // Link density (when HTML supplied): too many anchors vs body length.
    double link_penalty = 0.0;
    if (!html.empty()) {
        size_t a_count = 0;
        std::string lh = lower(html);
        size_t pos = 0;
        while ((pos = lh.find("<a ", pos)) != std::string::npos) { ++a_count; ++pos; }
        double density = (double)a_count / std::max<size_t>(1, tok_count);
        if (density > 0.05) link_penalty = std::min(0.4, (density - 0.05) * 2.0);
    }

    double base = 0.4 * length_score + 0.3 * sent_score + 0.3 * alpha_ratio;
    double q    = std::max(0.0, base - boiler_penalty - link_penalty);
    return std::min(1.0, q);
}

bool detect_paywall(std::string_view html, std::string_view text) {
    std::string l = lower(text);
    static const std::array<const char *, 7> needles = {
        "subscribe to continue", "subscribe to read", "to continue reading",
        "this article is for subscribers", "you've reached your free article limit",
        "create a free account to keep reading", "metered_paywall"
    };
    for (const auto * n : needles) if (l.find(n) != std::string::npos) return true;
    std::string lh = lower(html);
    if (lh.find("paywall") != std::string::npos) return true;
    if (lh.find("\"isaccessibleforfree\":false") != std::string::npos) return true;
    return false;
}

bool detect_error_page(std::string_view text, int http_status) {
    if (http_status >= 400 && http_status < 600) return true;
    std::string l = lower(text);
    for (const char * n : {"page not found","404 not found","403 forbidden",
                            "internal server error","this page does not exist"}) {
        if (l.find(n) != std::string::npos) return true;
    }
    return false;
}

std::vector<std::string> topic_tags(std::string_view text) {
    std::string l = lower(text);
    std::vector<std::string> tags;
    auto hit = [&](const std::vector<const char *> & needles, const char * tag) {
        for (const auto * n : needles) if (l.find(n) != std::string::npos) {
            tags.emplace_back(tag); return;
        }
    };
    hit({"breaking news","reporters","correspondent","editorial board"}, "news");
    hit({"abstract","doi:","methods","conclusion","figure 1"},           "science");
    hit({"function ","class ","import ","const ","void ","def "},        "code");
    hit({"sponsored","advertisement","promotional partner"},             "ad");
    hit({"agenda","minutes","attendees","action items"},                 "meeting");
    hit({"recipe","ingredients","preheat","tablespoon"},                 "recipe");
    return tags;
}

TriageReport triage(std::string_view html, std::string_view text,
                    int http_status, double quality_floor) {
    TriageReport r;
    r.language    = detect_language(text);
    r.quality     = estimate_quality(text, html);
    r.paywall     = detect_paywall(html, text);
    r.error_page  = detect_error_page(text, http_status);
    r.topic_tags  = topic_tags(text);

    if (r.error_page) { r.drop = true; r.reason = "error page"; return r; }
    if (r.paywall)    { r.drop = true; r.reason = "paywall";    return r; }
    if (r.quality < quality_floor) {
        r.drop = true;
        r.reason = "quality below floor";
    }
    return r;
}

}
