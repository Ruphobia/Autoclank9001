// SPDX-License-Identifier: GPL-3.0-or-later
#include "dictionary.hpp"

#include <nlohmann/json.hpp>

#include <cctype>
#include <cstdio>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dictionary {
namespace {

constexpr const char * kWebsterPath = "resources/dictionary/dictionary.json";
constexpr const char * kWordNetPath = "resources/dictionary/wordnet.json";
constexpr const char * kMobyPath    = "resources/dictionary/mthesaur.txt";

struct WordNetSense {
    std::string pos;
    std::string def;
};

std::once_flag                                                g_once;
std::unordered_map<std::string, std::string>                  g_webster;
std::unordered_map<std::string, std::vector<WordNetSense>>    g_wordnet;
// Moby Thesaurus II: root word -> raw comma-separated synonym list. The
// list is kept unsplit (one string per root) and cut on demand; splitting
// 2.5M synonyms up front would triple the load time and memory for a
// lookup that usually wants the first dozen.
std::unordered_map<std::string, std::string>                  g_moby;

std::string to_lower(std::string_view s) {
    std::string r;
    r.reserve(s.size());
    for (char c : s) {
        r.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return r;
}

nlohmann::json load_json(const char * path) {
    std::ifstream f(path);
    if (!f) {
        throw std::runtime_error(std::string("dictionary: cannot open ") + path);
    }
    nlohmann::json j;
    f >> j;
    return j;
}

void load_webster() {
    auto j = load_json(kWebsterPath);
    if (!j.is_object()) {
        throw std::runtime_error("dictionary: Webster JSON must be an object");
    }
    g_webster.reserve(j.size());
    for (auto it = j.begin(); it != j.end(); ++it) {
        if (it.value().is_string()) {
            g_webster.emplace(to_lower(it.key()), it.value().get<std::string>());
        }
    }
}

void load_wordnet() {
    auto j = load_json(kWordNetPath);
    if (!j.is_object()) {
        throw std::runtime_error("dictionary: WordNet JSON must be an object");
    }
    g_wordnet.reserve(j.size());
    for (auto it = j.begin(); it != j.end(); ++it) {
        if (!it.value().is_array()) continue;
        std::vector<WordNetSense> senses;
        senses.reserve(it.value().size());
        for (const auto & s : it.value()) {
            if (!s.is_object()) continue;
            WordNetSense ws;
            if (s.contains("pos") && s["pos"].is_string()) ws.pos = s["pos"].get<std::string>();
            if (s.contains("def") && s["def"].is_string()) ws.def = s["def"].get<std::string>();
            if (!ws.def.empty()) senses.push_back(std::move(ws));
        }
        if (!senses.empty()) {
            g_wordnet.emplace(to_lower(it.key()), std::move(senses));
        }
    }
}

// Lenient by design: the thesaurus is an enrichment, not a prerequisite.
// A checkout without the file keeps every existing feature working;
// synonyms() just returns empty.
void load_moby() {
    std::ifstream f(kMobyPath);
    if (!f) {
        std::fprintf(stderr,
                     "dictionary: %s missing; synonyms() disabled\n",
                     kMobyPath);
        return;
    }
    g_moby.reserve(31000);
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const auto comma = line.find(',');
        if (comma == std::string::npos || comma == 0) continue;
        g_moby.emplace(to_lower(line.substr(0, comma)),
                       line.substr(comma + 1));
    }
}

void load_once() {
    std::call_once(g_once, []{
        load_webster();
        load_wordnet();
        load_moby();
    });
}

}

void init() {
    load_once();
}

std::vector<Entry> lookup(std::string_view word) {
    load_once();
    std::vector<Entry> out;
    const std::string key = to_lower(word);

    if (auto it = g_webster.find(key); it != g_webster.end()) {
        out.push_back({"Webster 1913", "", it->second});
    }
    if (auto it = g_wordnet.find(key); it != g_wordnet.end()) {
        for (const auto & s : it->second) {
            out.push_back({"WordNet", s.pos, s.def});
        }
    }
    return out;
}

std::vector<std::string> synonyms(std::string_view word, int max_results) {
    load_once();
    std::vector<std::string> out;
    const auto it = g_moby.find(to_lower(word));
    if (it == g_moby.end()) return out;
    const std::string & list = it->second;
    std::size_t pos = 0;
    while (pos < list.size() &&
           (max_results <= 0 || static_cast<int>(out.size()) < max_results)) {
        std::size_t comma = list.find(',', pos);
        if (comma == std::string::npos) comma = list.size();
        if (comma > pos) out.emplace_back(list.substr(pos, comma - pos));
        pos = comma + 1;
    }
    return out;
}

}
