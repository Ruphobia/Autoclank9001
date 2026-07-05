#include "composer.hpp"

#include "../022_project_memory/project_memory.hpp"
#include "../023_personal_memory/personal_memory.hpp"

#include <algorithm>
#include <cstdio>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace memory_composer {
namespace {

std::string fmt_confidence(double c) {
    if (c < 0.0) c = 0.0;
    if (c > 1.0) c = 1.0;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%.2f", c);
    return buf;
}

}

const char * to_string(Scope s) {
    return s == Scope::Personal ? "personal" : "project";
}

const char * to_string(EntryKind k) {
    switch (k) {
        case EntryKind::Observation: return "observation";
        case EntryKind::Decision:    return "decision";
        case EntryKind::Preference:  return "preference";
        case EntryKind::Fact:        return "fact";
    }
    return "observation";
}

EntryKind kind_from_string(std::string_view s) {
    if (s == "observation") return EntryKind::Observation;
    if (s == "decision")    return EntryKind::Decision;
    if (s == "preference")  return EntryKind::Preference;
    if (s == "fact")        return EntryKind::Fact;
    return EntryKind::Observation;
}

std::vector<std::string> validate(const Draft & d) {
    std::vector<std::string> warns;
    bool only_ws = true;
    for (char c : d.content) if (c != ' ' && c != '\t' && c != '\n' && c != '\r') { only_ws = false; break; }
    if (only_ws) warns.emplace_back("content is empty");
    if (d.confidence < 0.0 || d.confidence > 1.0)
        warns.emplace_back("confidence must be in [0, 1]");
    for (const auto & t : d.tags) {
        if (t.find(' ') != std::string::npos)
            warns.emplace_back("tag contains a space: '" + t + "' (use '-' or '_' as separator)");
    }
    return warns;
}

std::string format_source(std::string_view source_link, double confidence) {
    std::ostringstream os;
    os << "composer:confidence=" << fmt_confidence(confidence);
    if (!source_link.empty()) os << " link=" << source_link;
    return os.str();
}

Submission submit(const Draft & d) {
    auto warns = validate(d);
    if (!warns.empty()) {
        std::string msg = "memory_composer::submit: invalid draft:";
        for (const auto & w : warns) msg += "\n  - " + w;
        throw std::runtime_error(msg);
    }
    Submission s;
    s.scope           = d.scope;
    s.canonical_source = format_source(d.source_link, d.confidence);
    if (d.scope == Scope::Project) {
        s.id = project_memory::add(to_string(d.kind), d.content, d.tags, s.canonical_source);
    } else {
        s.id = personal_memory::add(to_string(d.kind), d.content, d.tags, s.canonical_source);
    }
    return s;
}

void edit(Scope scope, int64_t id,
          std::string_view content,
          const std::vector<std::string> & tags,
          std::string_view source_link,
          double confidence) {
    std::string canonical = format_source(source_link, confidence);
    if (scope == Scope::Project) project_memory::update(id, content, tags, canonical);
    else                         personal_memory::update(id, content, tags, canonical);
}

}
