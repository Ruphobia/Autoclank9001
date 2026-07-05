#include "change_watcher.hpp"

#include <algorithm>
#include <regex>
#include <sstream>

namespace change_watcher {
namespace {

std::vector<std::string> split_lines(const std::string & s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == '\n') { out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

}

void init()     {}
void shutdown() {}

std::string apply_masks(std::string_view body_v,
                        const std::vector<NoiseMask> & masks) {
    std::string body{body_v};
    for (const auto & m : masks) {
        try {
            std::regex r(m.pattern, std::regex::ECMAScript);
            body = std::regex_replace(body, r, m.replacement);
        } catch (...) {
            // Ignore malformed masks; treat as no-op.
        }
    }
    return body;
}

ChangeReport compare(std::string_view before, std::string_view after,
                     const std::vector<NoiseMask> & masks) {
    ChangeReport r;
    r.masked_before = apply_masks(before, masks);
    r.masked_after  = apply_masks(after,  masks);

    auto bl = split_lines(r.masked_before);
    auto al = split_lines(r.masked_after);

    // Trim a common prefix and a common suffix; everything in the middle
    // is the change region.
    size_t pre = 0;
    while (pre < bl.size() && pre < al.size() && bl[pre] == al[pre]) ++pre;
    size_t suf = 0;
    while (suf + pre < bl.size() && suf + pre < al.size() &&
           bl[bl.size() - 1 - suf] == al[al.size() - 1 - suf]) ++suf;

    for (size_t i = 0; i < pre; ++i)
        r.diff.push_back({LineDiff::Kind::Same, bl[i]});
    for (size_t i = pre; i + suf < bl.size(); ++i)
        r.diff.push_back({LineDiff::Kind::Removed, bl[i]});
    for (size_t i = pre; i + suf < al.size(); ++i)
        r.diff.push_back({LineDiff::Kind::Added,   al[i]});
    for (size_t i = bl.size() - suf; i < bl.size(); ++i)
        r.diff.push_back({LineDiff::Kind::Same, bl[i]});

    size_t same    = 0, total = 0;
    for (const auto & d : r.diff) {
        ++total;
        if (d.kind == LineDiff::Kind::Same) ++same;
    }
    r.similarity = total ? (double)same / (double)total : 1.0;
    r.changed    = r.masked_before != r.masked_after;
    return r;
}

}
