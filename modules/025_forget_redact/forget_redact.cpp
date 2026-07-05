#include "forget_redact.hpp"

#include "../022_project_memory/project_memory.hpp"
#include "../023_personal_memory/personal_memory.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace forget_redact {
namespace {

namespace fs = std::filesystem;

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

fs::path personal_dir() {
    if (const char * e = std::getenv("TOOL_PERSONAL_DIR")) return fs::path(e);
    if (const char * h = std::getenv("HOME"))              return fs::path(h) / ".tool" / "personal";
    return fs::current_path() / ".tool" / "personal";
}

fs::path store_dir(Scope scope, std::string_view project_root) {
    if (scope == Scope::Personal) return personal_dir();
    fs::path root = project_root.empty() ? fs::current_path()
                                         : fs::path(std::string(project_root));
    return root / ".tool" / "memory";
}

fs::path tomb_dir(Scope scope, std::string_view project_root) {
    return store_dir(scope, project_root) / "tombstones";
}
fs::path audit_path(Scope scope, std::string_view project_root) {
    return store_dir(scope, project_root) / "audit.jsonl";
}

int64_t next_tomb_id(const fs::path & dir) {
    if (!fs::exists(dir)) return 1;
    int64_t max_id = 0;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        if (e.path().extension() != ".json") continue;
        try { max_id = std::max(max_id, (int64_t)std::stoll(e.path().stem().string())); }
        catch (...) {}
    }
    return max_id + 1;
}

void append_audit(Scope scope, std::string_view project_root,
                  const nlohmann::json & rec) {
    fs::path p = audit_path(scope, project_root);
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::app);
    if (!o) throw std::runtime_error("forget_redact: cannot append audit " + p.string());
    o << rec.dump() << "\n";
}

Tombstone tomb_from_json(const nlohmann::json & j) {
    Tombstone t;
    t.tombstone_id  = j.value("tombstone_id",  (int64_t)0);
    t.original_id   = j.value("original_id",   (int64_t)0);
    t.tombstoned_at = j.value("tombstoned_at", (int64_t)0);
    t.reason        = j.value("reason",        "");
    t.category      = j.value("category",      "");
    t.content       = j.value("content",       "");
    if (j.contains("tags")) t.tags = j["tags"].get<std::vector<std::string>>();
    t.source        = j.value("source",        "");
    return t;
}

nlohmann::json tomb_to_json(const Tombstone & t) {
    nlohmann::json j;
    j["tombstone_id"]  = t.tombstone_id;
    j["original_id"]   = t.original_id;
    j["tombstoned_at"] = t.tombstoned_at;
    j["reason"]        = t.reason;
    j["category"]      = t.category;
    j["content"]       = t.content;
    j["tags"]          = t.tags;
    j["source"]        = t.source;
    return j;
}

fs::path tomb_path(Scope scope, int64_t id, std::string_view project_root) {
    char name[32]; std::snprintf(name, sizeof(name), "%08lld.json", (long long)id);
    return tomb_dir(scope, project_root) / name;
}

}

const char * to_string(Scope s) {
    return s == Scope::Personal ? "personal" : "project";
}

void soft_forget(Scope scope, int64_t id,
                 std::string_view reason,
                 std::string_view project_root,
                 int64_t trash_window_seconds) {
    Tombstone t;
    t.original_id   = id;
    t.tombstoned_at = now_unix();
    t.reason        = std::string(reason);

    if (scope == Scope::Project) {
        auto e = project_memory::get(id);
        if (!e) throw std::runtime_error("soft_forget: live entry not found");
        t.category = e->category; t.content = e->content;
        t.tags     = e->tags;     t.source  = e->source;
    } else {
        auto e = personal_memory::get(id);
        if (!e) throw std::runtime_error("soft_forget: live entry not found");
        t.category = e->category; t.content = e->content;
        t.tags     = e->tags;     t.source  = e->source;
    }

    fs::path dir = tomb_dir(scope, project_root);
    fs::create_directories(dir);
    t.tombstone_id = next_tomb_id(dir);
    fs::path p     = tomb_path(scope, t.tombstone_id, project_root);
    {
        std::ofstream o(p, std::ios::binary | std::ios::trunc);
        o << tomb_to_json(t).dump(2);
    }

    if (scope == Scope::Project) project_memory::forget(id);
    else                         personal_memory::forget(id);

    nlohmann::json rec = {
        {"op",           "soft_forget"},
        {"scope",        to_string(scope)},
        {"original_id",  id},
        {"tombstone_id", t.tombstone_id},
        {"reason",       std::string(reason)},
        {"ts",           t.tombstoned_at}
    };
    append_audit(scope, project_root, rec);

    purge_older(scope, t.tombstoned_at - trash_window_seconds, project_root);
}

void hard_delete(Scope scope, int64_t id, std::string_view project_root) {
    // Removing whichever exists is fine; ignore "not found" so the
    // operation is idempotent against half-states.
    if (scope == Scope::Project) {
        try { project_memory::forget(id); } catch (...) {}
    } else {
        try { personal_memory::forget(id); } catch (...) {}
    }
    fs::path dir = tomb_dir(scope, project_root);
    if (fs::exists(dir)) {
        for (const auto & e : fs::directory_iterator(dir)) {
            if (!e.is_regular_file()) continue;
            try {
                std::ifstream in(e.path());
                nlohmann::json j; in >> j;
                if ((int64_t)j.value("original_id", 0) == id) fs::remove(e.path());
            } catch (...) {}
        }
    }
    nlohmann::json rec = {
        {"op",          "hard_delete"},
        {"scope",       to_string(scope)},
        {"original_id", id},
        {"ts",          now_unix()}
    };
    append_audit(scope, project_root, rec);
}

size_t redact(Scope scope, int64_t id,
              std::string_view pattern, std::string_view project_root,
              std::string_view replacement) {
    std::string content, source;
    std::vector<std::string> tags;
    if (scope == Scope::Project) {
        auto e = project_memory::get(id);
        if (!e) throw std::runtime_error("redact: live entry not found");
        content = e->content; tags = e->tags; source = e->source;
    } else {
        auto e = personal_memory::get(id);
        if (!e) throw std::runtime_error("redact: live entry not found");
        content = e->content; tags = e->tags; source = e->source;
    }
    std::regex re{std::string(pattern)};
    size_t matches = std::distance(
        std::sregex_iterator(content.begin(), content.end(), re),
        std::sregex_iterator());
    std::string after = std::regex_replace(content, re, std::string(replacement));

    if (scope == Scope::Project) project_memory::update(id, after, tags, source);
    else                         personal_memory::update(id, after, tags, source);

    nlohmann::json rec = {
        {"op",      "redact"},
        {"scope",   to_string(scope)},
        {"id",      id},
        {"pattern", std::string(pattern)},
        {"matches", matches},
        {"ts",      now_unix()}
    };
    append_audit(scope, project_root, rec);
    return matches;
}

size_t purge_older(Scope scope, int64_t older_than_unix,
                   std::string_view project_root) {
    fs::path dir = tomb_dir(scope, project_root);
    if (!fs::exists(dir)) return 0;
    size_t purged = 0;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file() || e.path().extension() != ".json") continue;
        try {
            std::ifstream in(e.path());
            nlohmann::json j; in >> j;
            int64_t ts = j.value("tombstoned_at", (int64_t)0);
            if (ts > 0 && ts < older_than_unix) {
                fs::remove(e.path());
                ++purged;
            }
        } catch (...) {}
    }
    if (purged) {
        nlohmann::json rec = {
            {"op",            "purge_older"},
            {"scope",         to_string(scope)},
            {"older_than",    older_than_unix},
            {"purged_count",  purged},
            {"ts",            now_unix()}
        };
        append_audit(scope, project_root, rec);
    }
    return purged;
}

std::vector<Tombstone> list_tombstones(Scope scope, std::string_view project_root) {
    std::vector<Tombstone> out;
    fs::path dir = tomb_dir(scope, project_root);
    if (!fs::exists(dir)) return out;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file() || e.path().extension() != ".json") continue;
        try {
            std::ifstream in(e.path());
            nlohmann::json j; in >> j;
            out.push_back(tomb_from_json(j));
        } catch (...) {}
    }
    std::sort(out.begin(), out.end(),
              [](const Tombstone & a, const Tombstone & b){ return a.tombstone_id < b.tombstone_id; });
    return out;
}

std::optional<Tombstone> read_tombstone(Scope scope, int64_t tombstone_id,
                                        std::string_view project_root) {
    fs::path p = tomb_path(scope, tombstone_id, project_root);
    if (!fs::exists(p)) return std::nullopt;
    try {
        std::ifstream in(p);
        nlohmann::json j; in >> j;
        return tomb_from_json(j);
    } catch (...) { return std::nullopt; }
}

int64_t restore(Scope scope, int64_t tombstone_id, std::string_view project_root) {
    auto t = read_tombstone(scope, tombstone_id, project_root);
    if (!t) throw std::runtime_error("restore: no such tombstone");
    int64_t new_id = 0;
    if (scope == Scope::Project)
        new_id = project_memory::add(t->category, t->content, t->tags, t->source);
    else
        new_id = personal_memory::add(t->category, t->content, t->tags, t->source);
    std::error_code ec;
    fs::remove(tomb_path(scope, tombstone_id, project_root), ec);
    nlohmann::json rec = {
        {"op",            "restore"},
        {"scope",         to_string(scope)},
        {"tombstone_id",  tombstone_id},
        {"new_live_id",   new_id},
        {"ts",            now_unix()}
    };
    append_audit(scope, project_root, rec);
    return new_id;
}

}
