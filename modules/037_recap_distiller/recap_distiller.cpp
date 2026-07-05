#include "recap_distiller.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace recap_distiller {
namespace {

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

// Split text into rough sentences on . ? ! and newlines.
std::vector<std::string> split_sentences(const std::string & body) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : body) {
        cur.push_back(c);
        if (c == '.' || c == '!' || c == '?' || c == '\n') {
            // Trim and emit.
            std::string s = cur;
            while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
            size_t b = 0; while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
            s.erase(0, b);
            if (!s.empty()) out.push_back(s);
            cur.clear();
        }
    }
    if (!cur.empty()) {
        std::string s = cur;
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
        size_t b = 0; while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
        s.erase(0, b);
        if (!s.empty()) out.push_back(s);
    }
    return out;
}

struct Rule {
    const char *           phrase;
    memory_composer::EntryKind kind;
    double                 confidence;
};

const Rule kRules[] = {
    // Decisions
    {"decided to",         memory_composer::EntryKind::Decision,   0.85},
    {"we'll go with",      memory_composer::EntryKind::Decision,   0.80},
    {"chose to",           memory_composer::EntryKind::Decision,   0.80},
    {"settled on",         memory_composer::EntryKind::Decision,   0.80},
    {"decision:",          memory_composer::EntryKind::Decision,   0.90},

    // Preferences
    {"i prefer",           memory_composer::EntryKind::Preference, 0.85},
    {"i like",             memory_composer::EntryKind::Preference, 0.70},
    {"i hate",             memory_composer::EntryKind::Preference, 0.80},
    {"i never use",        memory_composer::EntryKind::Preference, 0.85},
    {"i always use",       memory_composer::EntryKind::Preference, 0.85},
    {"please don't",       memory_composer::EntryKind::Preference, 0.80},
    {"do not use",         memory_composer::EntryKind::Preference, 0.75},
    {"from now on",        memory_composer::EntryKind::Preference, 0.80},

    // Facts
    {"i learned",          memory_composer::EntryKind::Fact,       0.75},
    {"til ",               memory_composer::EntryKind::Fact,       0.75},
    {"note that",          memory_composer::EntryKind::Fact,       0.65},
    {"fact:",              memory_composer::EntryKind::Fact,       0.90},
    {"turns out",          memory_composer::EntryKind::Fact,       0.65},
    {"the actual answer",  memory_composer::EntryKind::Fact,       0.70},

    // Observations: catch-all-ish; lower confidence.
    {"observed that",      memory_composer::EntryKind::Observation, 0.70},
    {"i noticed",          memory_composer::EntryKind::Observation, 0.70},
    {"i saw",              memory_composer::EntryKind::Observation, 0.60},
};

std::vector<std::string> tags_for(memory_composer::EntryKind k) {
    switch (k) {
        case memory_composer::EntryKind::Decision:    return {"recap","decision"};
        case memory_composer::EntryKind::Preference:  return {"recap","preference"};
        case memory_composer::EntryKind::Fact:        return {"recap","fact"};
        case memory_composer::EntryKind::Observation: return {"recap","observation"};
    }
    return {"recap"};
}

}

void init()     {}
void shutdown() {}

std::vector<Candidate> distill(const std::vector<Turn> & transcript) {
    std::vector<Candidate> out;
    for (const auto & turn : transcript) {
        if (turn.speaker != Speaker::User) continue;
        std::string l = lower(turn.text);
        // Walk sentences so the captured content is the local sentence,
        // not the whole turn.
        auto sentences = split_sentences(turn.text);
        std::vector<std::string> l_sent;
        l_sent.reserve(sentences.size());
        for (const auto & s : sentences) l_sent.push_back(lower(s));

        for (const auto & rule : kRules) {
            for (size_t i = 0; i < l_sent.size(); ++i) {
                if (l_sent[i].find(rule.phrase) == std::string::npos) continue;
                Candidate c;
                c.draft.scope       = memory_composer::Scope::Project;
                c.draft.kind        = rule.kind;
                c.draft.content     = sentences[i];
                c.draft.tags        = tags_for(rule.kind);
                std::ostringstream src;
                src << "session:" << (turn.session_id.empty() ? "(unknown)" : turn.session_id)
                    << ":turn:" << turn.index;
                c.draft.source_link = src.str();
                c.draft.confidence  = rule.confidence;
                c.trigger           = rule.phrase;
                c.turn_index        = (size_t)turn.index;
                out.push_back(std::move(c));
            }
        }
    }
    return out;
}

}
