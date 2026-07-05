#include "footnote_manager.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <unordered_map>

namespace footnote_manager {
namespace {

const std::regex & re_def() {
    static const std::regex r(R"(\[\^([^\]]+)\]:\s*([^\n]+))");
    return r;
}
const std::regex & re_ref() {
    static const std::regex r(R"(\[\^([^\]]+)\])");
    return r;
}

bool is_def_at(const std::string & s, size_t pos) {
    auto br = s.find(']', pos);
    if (br == std::string::npos) return false;
    return br + 1 < s.size() && s[br + 1] == ':';
}

}

void init()     {}
void shutdown() {}

std::vector<Footnote> extract(std::string_view doc_v) {
    std::string doc{doc_v};
    std::unordered_map<std::string, Footnote> by_id;
    std::vector<std::string> order;

    // Definitions.
    for (auto it = std::sregex_iterator(doc.begin(), doc.end(), re_def());
         it != std::sregex_iterator(); ++it) {
        std::string id   = (*it)[1].str();
        std::string text = (*it)[2].str();
        if (!by_id.count(id)) { by_id[id] = Footnote{id, text, {}}; order.push_back(id); }
        else                  { by_id[id].text = text; }
    }
    // References (skip definitions).
    for (auto it = std::sregex_iterator(doc.begin(), doc.end(), re_ref());
         it != std::sregex_iterator(); ++it) {
        size_t pos = (size_t)it->position(0);
        if (is_def_at(doc, pos)) continue;
        std::string id = (*it)[1].str();
        if (!by_id.count(id)) { by_id[id] = Footnote{id, "", {}}; order.push_back(id); }
        by_id[id].ref_positions.push_back(pos);
    }
    std::vector<Footnote> out;
    for (const auto & id : order) out.push_back(by_id[id]);
    return out;
}

std::string renumber(std::string_view doc_v) {
    std::string doc{doc_v};
    auto fns = extract(doc);

    // Build id -> new number, in order of first reference (falling back
    // to definition order for unreferenced footnotes).
    std::unordered_map<std::string, int> renum;
    std::vector<std::string> first_ref_order;
    std::vector<std::string> def_only_order;
    for (const auto & f : fns) {
        if (!f.ref_positions.empty()) first_ref_order.push_back(f.id);
        else                          def_only_order.push_back(f.id);
    }
    int n = 0;
    for (const auto & id : first_ref_order) renum[id] = ++n;
    for (const auto & id : def_only_order)  renum[id] = ++n;

    // First pass: rewrite references.
    auto rewrite_refs = [&](const std::string & s) {
        std::string out;
        size_t pos = 0;
        for (auto it = std::sregex_iterator(s.begin(), s.end(), re_ref());
             it != std::sregex_iterator(); ++it) {
            size_t mpos = (size_t)it->position(0);
            size_t mlen = (size_t)it->length(0);
            if (is_def_at(s, mpos)) continue;
            out.append(s, pos, mpos - pos);
            std::string id = (*it)[1].str();
            auto rit = renum.find(id);
            out.append("[^").append(rit != renum.end() ? std::to_string(rit->second) : id).append("]");
            pos = mpos + mlen;
        }
        out.append(s, pos, std::string::npos);
        return out;
    };

    auto rewrite_defs = [&](const std::string & s) {
        std::string out;
        size_t pos = 0;
        for (auto it = std::sregex_iterator(s.begin(), s.end(), re_def());
             it != std::sregex_iterator(); ++it) {
            size_t mpos = (size_t)it->position(0);
            size_t mlen = (size_t)it->length(0);
            out.append(s, pos, mpos - pos);
            std::string id   = (*it)[1].str();
            std::string text = (*it)[2].str();
            auto rit = renum.find(id);
            out.append("[^")
               .append(rit != renum.end() ? std::to_string(rit->second) : id)
               .append("]: ").append(text);
            pos = mpos + mlen;
        }
        out.append(s, pos, std::string::npos);
        return out;
    };

    std::string a = rewrite_refs(doc);
    std::string b = rewrite_defs(a);
    return b;
}

std::string apply_ibid(std::string_view doc_v) {
    std::string doc{doc_v};
    std::string out;
    std::string previous_id;
    size_t pos = 0;
    for (auto it = std::sregex_iterator(doc.begin(), doc.end(), re_ref());
         it != std::sregex_iterator(); ++it) {
        size_t mpos = (size_t)it->position(0);
        size_t mlen = (size_t)it->length(0);
        if (is_def_at(doc, mpos)) continue;
        out.append(doc, pos, mpos - pos);
        std::string id = (*it)[1].str();
        if (!previous_id.empty() && id == previous_id) {
            out.append("ibid.");
        } else {
            out.append("[^").append(id).append("]");
        }
        previous_id = id;
        pos = mpos + mlen;
    }
    out.append(doc, pos, std::string::npos);
    return out;
}

}
