#include "citation_key.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>

namespace citation_key {
namespace {

std::string ascii_lower_strip(std::string_view in) {
    std::string out;
    out.reserve(in.size());
    for (char c : in) {
        unsigned char uc = (unsigned char)c;
        if (std::isalnum(uc)) out.push_back((char)std::tolower(uc));
    }
    return out;
}

const std::unordered_set<std::string> & stoplist() {
    static const std::unordered_set<std::string> s = {
        "the","a","an","of","in","on","at","to","for","with","by",
        "and","or","but","is","are","was","were","be","been","being",
        "from","into","over","under","without","within","across"
    };
    return s;
}

std::string year_2digit(const nlohmann::json & csl) {
    std::string y;
    if (csl.contains("issued")) {
        const auto & iss = csl["issued"];
        if (iss.contains("date-parts") && iss["date-parts"].is_array() &&
            !iss["date-parts"].empty() && iss["date-parts"][0].is_array() &&
            !iss["date-parts"][0].empty()) {
            const auto & v = iss["date-parts"][0][0];
            if (v.is_number_integer()) y = std::to_string(v.get<int>());
            else if (v.is_string())    y = v.get<std::string>();
        }
        if (y.empty() && iss.contains("literal") && iss["literal"].is_string())
            y = iss["literal"].get<std::string>();
    }
    if (y.size() >= 2) return y.substr(y.size() - 2);
    return y;
}

std::string year_full(const nlohmann::json & csl) {
    if (csl.contains("issued")) {
        const auto & iss = csl["issued"];
        if (iss.contains("date-parts") && iss["date-parts"].is_array() &&
            !iss["date-parts"].empty() && iss["date-parts"][0].is_array() &&
            !iss["date-parts"][0].empty()) {
            const auto & v = iss["date-parts"][0][0];
            if (v.is_number_integer()) return std::to_string(v.get<int>());
            if (v.is_string())         return v.get<std::string>();
        }
        if (iss.contains("literal") && iss["literal"].is_string())
            return iss["literal"].get<std::string>();
    }
    return "nodate";
}

std::string journal_short(std::string_view full) {
    // Pick the first letters of each significant word in the container title.
    std::string buf;
    std::string ct{full};
    std::istringstream is(ct);
    std::string w;
    while (is >> w) {
        std::string n = ascii_lower_strip(w);
        if (n.size() < 2) continue;
        if (stoplist().count(n)) continue;
        buf.push_back(n.front());
        if (buf.size() >= 4) break;
    }
    if (buf.empty()) buf = "nojrnl";
    return buf;
}

}

const char * to_string(Template t) {
    switch (t) {
        case Template::AuthorYearTitleWord: return "author-year-title";
        case Template::AuthorYear:          return "author-year";
        case Template::AlphaThreeYear:      return "alpha-3-year";
        case Template::AuthorYearJournal:   return "author-year-journal";
    }
    return "author-year-title";
}

std::string sanitize_token(std::string_view in) {
    return ascii_lower_strip(in);
}

std::string first_author_family(const nlohmann::json & csl) {
    if (!csl.contains("author") || !csl["author"].is_array() || csl["author"].empty())
        return "anon";
    const auto & a = csl["author"][0];
    if (!a.is_object()) return "anon";
    std::string fam = a.value("family", "");
    if (fam.empty()) fam = a.value("literal", "");
    fam = ascii_lower_strip(fam);
    return fam.empty() ? "anon" : fam;
}

std::string first_title_word(std::string_view title) {
    std::string t{title};
    std::istringstream is(t);
    std::string w;
    while (is >> w) {
        std::string n = ascii_lower_strip(w);
        if (n.size() < 4) continue;
        if (stoplist().count(n)) continue;
        return n;
    }
    return {};
}

namespace {

std::string next_suffix(size_t k) {
    // 0 -> "a", 25 -> "z", 26 -> "aa", 27 -> "ab", ...
    std::string s;
    ++k;     // shift so 0 produces a single char "a"
    while (k > 0) {
        --k;
        s.insert(s.begin(), (char)('a' + (k % 26)));
        k /= 26;
    }
    return s;
}

}

std::string generate(const nlohmann::json & csl, Template tmpl,
                     const std::unordered_set<std::string> & existing) {
    std::string author = first_author_family(csl);
    std::string year_f = year_full(csl);
    std::string year_s = year_2digit(csl);
    std::string title  = first_title_word(csl.value("title", ""));
    std::string jshort = journal_short(csl.value("container-title", ""));

    std::string base;
    switch (tmpl) {
        case Template::AuthorYearTitleWord:
            base = author + year_f + (title.empty() ? "" : title);
            break;
        case Template::AuthorYear:
            base = author + year_f;
            break;
        case Template::AlphaThreeYear: {
            std::string fam = first_author_family(csl);
            if (fam.size() > 3) fam = fam.substr(0, 3);
            if (!fam.empty()) fam[0] = (char)std::toupper((unsigned char)fam[0]);
            base = fam + year_s;
            break;
        }
        case Template::AuthorYearJournal:
            base = author + year_f + jshort;
            break;
    }
    if (base.empty()) base = "key";

    if (!existing.count(base)) return base;
    for (size_t k = 0; k < 1000; ++k) {
        std::string cand = base + next_suffix(k);
        if (!existing.count(cand)) return cand;
    }
    return base + "_uniq";
}

}
