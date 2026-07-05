#include "regex_designer.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <set>
#include <sstream>

namespace regex_designer {
namespace {

void collect_named_groups(std::string_view pattern,
                          std::vector<std::string> & names) {
    // Both flavors: (?P<name>...) PCRE, (?<name>...) ECMA. Lazy scan.
    for (size_t i = 0; i + 2 < pattern.size(); ++i) {
        if (pattern[i] != '(' || pattern[i + 1] != '?') continue;
        size_t j = i + 2;
        if (j < pattern.size() && pattern[j] == 'P') ++j;
        if (j >= pattern.size() || pattern[j] != '<') continue;
        size_t end = pattern.find('>', j + 1);
        if (end == std::string_view::npos) continue;
        names.emplace_back(pattern.substr(j + 1, end - j - 1));
    }
}

// libstdc++'s std::regex does not accept (?<name>...) / (?P<name>...);
// rewrite those to plain unnamed groups so the regex compiles. The
// caller attaches names from the parsed pattern after the fact.
std::string strip_named_groups(std::string_view pattern_v) {
    std::string p{pattern_v};
    std::string out;
    out.reserve(p.size());
    for (size_t i = 0; i < p.size(); ++i) {
        if (i + 3 < p.size() && p[i] == '(' && p[i + 1] == '?' &&
            ((p[i + 2] == '<') ||
             (p[i + 2] == 'P' && p[i + 3] == '<'))) {
            size_t j = (p[i + 2] == 'P') ? i + 4 : i + 3;
            size_t end = p.find('>', j);
            if (end == std::string::npos) { out.push_back(p[i]); continue; }
            out.push_back('(');
            i = end;     // skip past '>'; loop ++ moves to the next byte
        } else {
            out.push_back(p[i]);
        }
    }
    return out;
}

}

const char * to_string(Dialect d) {
    switch (d) {
        case Dialect::Ecma:  return "ecma";
        case Dialect::Pcre:  return "pcre";
        case Dialect::Re2:   return "re2";
        case Dialect::Posix: return "posix";
    }
    return "ecma";
}

void init()     {}
void shutdown() {}

TestResult test(std::string_view pattern_v, std::string_view sample_v,
                bool case_insensitive, bool find_all) {
    TestResult r;
    std::string pattern{pattern_v};
    std::string sample{sample_v};

    std::vector<std::string> names;
    collect_named_groups(pattern, names);
    std::string compile_pattern = strip_named_groups(pattern);

    std::regex re;
    try {
        auto flags = std::regex::ECMAScript;
        if (case_insensitive) flags = flags | std::regex::icase;
        re = std::regex(compile_pattern, flags);
    } catch (const std::regex_error & ex) {
        r.valid_pattern = false;
        r.error         = ex.what();
        return r;
    }

    auto fill_match = [&](const std::cmatch & m) {
        MatchSpan ms;
        ms.start  = (size_t)m.position(0);
        ms.length = (size_t)m.length(0);
        ms.text   = m.str(0);
        for (size_t i = 1; i < m.size(); ++i) {
            Capture c;
            c.start  = (size_t)m.position((int)i);
            c.length = (size_t)m.length((int)i);
            c.text   = m.str((int)i);
            if (i - 1 < names.size()) c.name = names[i - 1];
            ms.captures.push_back(c);
        }
        r.matches.push_back(std::move(ms));
    };

    if (!find_all) {
        std::cmatch m;
        if (std::regex_search(sample.c_str(), sample.c_str() + sample.size(), m, re)) {
            fill_match(m);
        }
        return r;
    }

    const char * s   = sample.c_str();
    const char * end = s + sample.size();
    std::cmatch m;
    while (std::regex_search(s, end, m, re)) {
        fill_match(m);
        size_t off = (size_t)(m[0].second - sample.c_str());
        // Avoid infinite loop on zero-length matches.
        if (m.length(0) == 0) ++off;
        if (off >= sample.size()) break;
        s = sample.c_str() + off;
    }
    return r;
}

std::string translate(std::string_view pattern_v, Dialect from, Dialect to) {
    if (from == to) return std::string{pattern_v};
    std::string p{pattern_v};

    // Named groups: normalize on ECMA shape first, then emit in target.
    {
        std::string out;
        out.reserve(p.size());
        for (size_t i = 0; i < p.size(); ++i) {
            if (i + 3 < p.size() && p[i] == '(' && p[i + 1] == '?' &&
                p[i + 2] == 'P' && p[i + 3] == '<') {
                out += "(?<";
                i += 3;
            } else {
                out.push_back(p[i]);
            }
        }
        p = std::move(out);
    }
    if (to == Dialect::Pcre) {
        // ECMA -> PCRE: emit (?P<name>...)
        std::string out;
        out.reserve(p.size());
        for (size_t i = 0; i < p.size(); ++i) {
            if (i + 2 < p.size() && p[i] == '(' && p[i + 1] == '?' && p[i + 2] == '<') {
                out += "(?P<";
                i += 2;
            } else {
                out.push_back(p[i]);
            }
        }
        p = std::move(out);
    }

    // Possessive quantifiers: PCRE supports them; RE2 / ECMA / POSIX
    // do not. Drop the trailing '+' so the quantifier degrades to
    // greedy.
    if (to == Dialect::Re2 || to == Dialect::Ecma || to == Dialect::Posix) {
        std::string out;
        out.reserve(p.size());
        for (size_t i = 0; i < p.size(); ++i) {
            char c = p[i];
            out.push_back(c);
            if (i + 1 < p.size() && (c == '*' || c == '+' || c == '?' || c == '}') &&
                p[i + 1] == '+') {
                // Skip the possessive '+' marker.
                ++i;
            }
        }
        p = std::move(out);
    }

    return p;
}

std::string synthesize_from_examples(const std::vector<std::string> & positives) {
    if (positives.empty()) return {};
    // Pick the shortest example as the skeleton; for each character
    // position compare across all examples. Same byte -> literal
    // (regex-escaped); otherwise emit a "." wildcard.
    size_t L = positives[0].size();
    for (const auto & s : positives) L = std::min(L, s.size());
    if (L == 0) return ".*";

    auto needs_escape = [](char c) {
        switch (c) {
            case '\\': case '.': case '+': case '*': case '?':
            case '(': case ')': case '[': case ']': case '{': case '}':
            case '|': case '^': case '$':
                return true;
            default:
                return false;
        }
    };

    std::string out;
    out.reserve(L * 2 + 16);
    for (size_t i = 0; i < L; ++i) {
        char c = positives[0][i];
        bool all_same = true;
        for (const auto & s : positives) if (s[i] != c) { all_same = false; break; }
        if (all_same) {
            if (needs_escape(c)) out.push_back('\\');
            out.push_back(c);
        } else {
            out.push_back('.');
        }
    }
    // Variable-length tail.
    bool any_longer = false;
    for (const auto & s : positives) if (s.size() > L) { any_longer = true; break; }
    if (any_longer) out += ".*";
    return out;
}

}
