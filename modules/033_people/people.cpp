#include "people.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace people {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;

fs::path personal_dir() {
    if (const char * e = std::getenv("TOOL_PERSONAL_DIR")) return fs::path(e);
    if (const char * h = std::getenv("HOME"))              return fs::path(h) / ".tool" / "personal";
    return fs::current_path() / ".tool" / "personal";
}

fs::path file_for(Scope scope, std::string_view project_root) {
    if (scope == Scope::Personal) return personal_dir() / "people.json";
    fs::path root = project_root.empty() ? fs::current_path()
                                         : fs::path(std::string(project_root));
    return root / ".tool" / "people" / "people.json";
}

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

nlohmann::json load_doc(const fs::path & p) {
    if (!fs::exists(p)) return nlohmann::json{{"next_id", 1}, {"people", nlohmann::json::array()}};
    std::ifstream in(p);
    try {
        nlohmann::json j; in >> j;
        if (!j.contains("people") || !j["people"].is_array()) j["people"] = nlohmann::json::array();
        if (!j.contains("next_id")) j["next_id"] = 1;
        return j;
    } catch (...) {
        return nlohmann::json{{"next_id", 1}, {"people", nlohmann::json::array()}};
    }
}

void save_doc(const fs::path & p, const nlohmann::json & j) {
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    if (!o) throw std::runtime_error("people: cannot write " + p.string());
    o << j.dump(2);
}

Person from_json(const nlohmann::json & j) {
    Person p;
    p.id   = j.value("id",   (int64_t)0);
    p.name = j.value("name", "");
    if (j.contains("aliases"))        p.aliases        = j["aliases"].get<std::vector<std::string>>();
    p.role = j.value("role", "");
    if (j.contains("affiliations"))   p.affiliations   = j["affiliations"].get<std::vector<std::string>>();
    p.notes = j.value("notes", "");
    if (j.contains("linked_entries")) p.linked_entries = j["linked_entries"].get<std::vector<int64_t>>();
    return p;
}

nlohmann::json to_json(const Person & p) {
    return nlohmann::json{
        {"id",             p.id},
        {"name",           p.name},
        {"aliases",        p.aliases},
        {"role",           p.role},
        {"affiliations",   p.affiliations},
        {"notes",          p.notes},
        {"linked_entries", p.linked_entries},
    };
}

}

void init()     {}
void shutdown() {}

std::string store_path(Scope scope, std::string_view project_root) {
    return file_for(scope, project_root).string();
}

int64_t add(Scope scope, const Person & in, std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path p = file_for(scope, project_root);
    auto doc = load_doc(p);
    Person np = in;
    np.id = doc["next_id"].get<int64_t>();
    doc["next_id"] = np.id + 1;
    doc["people"].push_back(to_json(np));
    save_doc(p, doc);
    return np.id;
}

void update(Scope scope, int64_t id, const Person & in, std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path p = file_for(scope, project_root);
    auto doc = load_doc(p);
    bool found = false;
    for (auto & j : doc["people"]) {
        if ((int64_t)j.value("id", 0) == id) {
            Person np = in;
            np.id = id;
            j = to_json(np);
            found = true;
            break;
        }
    }
    if (!found) throw std::runtime_error("people::update: no such person id");
    save_doc(p, doc);
}

void forget(Scope scope, int64_t id, std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path p = file_for(scope, project_root);
    auto doc = load_doc(p);
    auto & arr = doc["people"];
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        if ((int64_t)it->value("id", 0) == id) { arr.erase(it); break; }
    }
    save_doc(p, doc);
}

std::optional<Person> get(Scope scope, int64_t id, std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    auto doc = load_doc(file_for(scope, project_root));
    for (const auto & j : doc["people"]) {
        if ((int64_t)j.value("id", 0) == id) return from_json(j);
    }
    return std::nullopt;
}

std::vector<Person> list(Scope scope, std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    auto doc = load_doc(file_for(scope, project_root));
    std::vector<Person> out;
    for (const auto & j : doc["people"]) out.push_back(from_json(j));
    std::sort(out.begin(), out.end(),
              [](const Person & a, const Person & b){ return a.id < b.id; });
    return out;
}

std::optional<Person> find(Scope scope, std::string_view query,
                           std::string_view project_root) {
    std::string q = lower(query);
    for (const auto & p : list(scope, project_root)) {
        if (lower(p.name) == q) return p;
        for (const auto & a : p.aliases) if (lower(a) == q) return p;
    }
    return std::nullopt;
}

std::vector<int64_t> find_mentions(Scope scope, std::string_view text,
                                   std::string_view project_root) {
    std::string hay = lower(text);
    auto folks = list(scope, project_root);

    std::vector<std::pair<size_t,int64_t>> hits;
    for (const auto & p : folks) {
        auto search_one = [&](const std::string & s) -> std::optional<size_t> {
            if (s.empty()) return std::nullopt;
            std::string n = lower(s);
            auto pos = hay.find(n);
            if (pos == std::string::npos) return std::nullopt;
            // Require word-ish boundary so "Lee" doesn't match "filed".
            auto is_word = [](char c){ return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; };
            bool left_ok  = pos == 0 || !is_word(hay[pos - 1]);
            bool right_ok = pos + n.size() == hay.size() || !is_word(hay[pos + n.size()]);
            return (left_ok && right_ok) ? std::optional<size_t>{pos} : std::nullopt;
        };
        std::optional<size_t> best;
        if (auto m = search_one(p.name)) best = *m;
        for (const auto & a : p.aliases) {
            if (auto m = search_one(a)) {
                if (!best || *m < *best) best = *m;
            }
        }
        if (best) hits.emplace_back(*best, p.id);
    }
    std::sort(hits.begin(), hits.end(),
              [](const auto & a, const auto & b){ return a.first < b.first; });
    std::vector<int64_t> out;
    for (auto & h : hits) out.push_back(h.second);
    return out;
}

void link_entry(Scope scope, int64_t person_id, int64_t entry_id,
                std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path p = file_for(scope, project_root);
    auto doc = load_doc(p);
    for (auto & j : doc["people"]) {
        if ((int64_t)j.value("id", 0) != person_id) continue;
        auto & arr = j["linked_entries"];
        if (!arr.is_array()) arr = nlohmann::json::array();
        for (const auto & e : arr) if ((int64_t)e == entry_id) return;
        arr.push_back(entry_id);
        save_doc(p, doc);
        return;
    }
    throw std::runtime_error("people::link_entry: no such person id");
}

}
