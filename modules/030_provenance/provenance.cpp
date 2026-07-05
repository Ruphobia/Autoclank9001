#include "provenance.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace provenance {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;

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
fs::path log_path(Scope scope, std::string_view project_root) {
    return store_dir(scope, project_root) / "provenance.jsonl";
}

int64_t next_id(Scope scope, std::string_view project_root) {
    fs::path p = log_path(scope, project_root);
    if (!fs::exists(p)) return 1;
    int64_t max_id = 0;
    std::ifstream in(p);
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try {
            auto j = nlohmann::json::parse(line);
            int64_t id = j.value("id", (int64_t)0);
            if (id > max_id) max_id = id;
        } catch (...) {}
    }
    return max_id + 1;
}

Record from_json(const nlohmann::json & j) {
    Record r;
    r.id       = j.value("id", (int64_t)0);
    r.ts       = j.value("ts", (int64_t)0);
    r.entry_id = j.value("entry_id", (int64_t)0);
    std::string k = j.value("kind", "manual");
    if      (k == "file")    r.kind = Kind::File;
    else if (k == "url")     r.kind = Kind::Url;
    else if (k == "chat")    r.kind = Kind::Chat;
    else if (k == "command") r.kind = Kind::Command;
    else if (k == "merge")   r.kind = Kind::Merge;
    else                     r.kind = Kind::Manual;
    r.ref     = j.value("ref",     "");
    r.hash    = j.value("hash",    "");
    r.excerpt = j.value("excerpt", "");
    return r;
}

nlohmann::json to_json(const Record & r) {
    return nlohmann::json{
        {"id",       r.id},
        {"ts",       r.ts},
        {"entry_id", r.entry_id},
        {"kind",     to_string(r.kind)},
        {"ref",      r.ref},
        {"hash",     r.hash},
        {"excerpt",  r.excerpt},
    };
}

void append(Scope scope, std::string_view project_root, const Record & r) {
    fs::path p = log_path(scope, project_root);
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::app);
    if (!o) throw std::runtime_error("provenance: cannot append " + p.string());
    o << to_json(r).dump() << "\n";
}

}

const char * to_string(Kind k) {
    switch (k) {
        case Kind::File:    return "file";
        case Kind::Url:     return "url";
        case Kind::Chat:    return "chat";
        case Kind::Command: return "command";
        case Kind::Manual:  return "manual";
        case Kind::Merge:   return "merge";
    }
    return "manual";
}

void init()     {}
void shutdown() {}

std::string hash_fnv64(std::string_view content) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (unsigned char c : content) {
        h ^= c;
        h *= 0x100000001b3ULL;
    }
    char buf[24];
    std::snprintf(buf, sizeof(buf), "fnv64:%016llx", (unsigned long long)h);
    return buf;
}

int64_t track(Scope scope, int64_t entry_id, Kind kind,
              std::string_view ref, std::string_view captured_content,
              std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    Record r;
    r.id       = next_id(scope, project_root);
    r.ts       = now_unix();
    r.entry_id = entry_id;
    r.kind     = kind;
    r.ref      = std::string(ref);
    r.hash     = hash_fnv64(captured_content);
    r.excerpt  = std::string(captured_content.substr(0, std::min<size_t>(captured_content.size(), 256)));
    append(scope, project_root, r);
    return r.id;
}

std::vector<Record> all(Scope scope, std::string_view project_root) {
    std::vector<Record> out;
    fs::path p = log_path(scope, project_root);
    if (!fs::exists(p)) return out;
    std::ifstream in(p);
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try { out.push_back(from_json(nlohmann::json::parse(line))); } catch (...) {}
    }
    return out;
}

std::vector<Record> for_entry(Scope scope, int64_t entry_id,
                              std::string_view project_root) {
    std::vector<Record> out;
    for (auto & r : all(scope, project_root)) {
        if (r.entry_id == entry_id) out.push_back(r);
    }
    return out;
}

std::optional<Record> get(Scope scope, int64_t id, std::string_view project_root) {
    for (auto & r : all(scope, project_root)) if (r.id == id) return r;
    return std::nullopt;
}

bool verify(const Record & r, std::string_view now_content) {
    return r.hash == hash_fnv64(now_content);
}

}
