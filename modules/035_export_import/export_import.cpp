#include "export_import.hpp"

#include "../022_project_memory/project_memory.hpp"
#include "../023_personal_memory/personal_memory.hpp"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace export_import {
namespace {

namespace fs = std::filesystem;

struct Snap {
    int64_t                  id;
    int64_t                  ts;
    std::string              category;
    std::string              content;
    std::vector<std::string> tags;
    std::string              source;
};

std::vector<Snap> snapshot(Scope scope) {
    std::vector<Snap> out;
    if (scope == Scope::Project) {
        for (auto & e : project_memory::list({}, {}, 100000))
            out.push_back({e.id, e.ts, e.category, e.content, e.tags, e.source});
    } else {
        for (auto & e : personal_memory::list({}, {}, 100000))
            out.push_back({e.id, e.ts, e.category, e.content, e.tags, e.source});
    }
    std::sort(out.begin(), out.end(),
              [](const Snap & a, const Snap & b){ return a.id < b.id; });
    return out;
}

nlohmann::json record_json(const Snap & s) {
    return nlohmann::json{
        {"id",       s.id},
        {"ts",       s.ts},
        {"category", s.category},
        {"content",  s.content},
        {"tags",     s.tags},
        {"source",   s.source},
    };
}

std::string iso_date(int64_t ts) {
    std::time_t t = (std::time_t)ts;
    std::tm tm{}; ::gmtime_r(&t, &tm);
    char buf[32]; std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return buf;
}

std::string mark_origin(const std::string & original_source, int64_t original_ts) {
    std::ostringstream os;
    os << "import:original-ts=" << original_ts;
    if (!original_source.empty()) os << " original-source=" << original_source;
    return os.str();
}

int64_t add_to_store(Scope scope, const std::string & category,
                     const std::string & content,
                     const std::vector<std::string> & tags,
                     const std::string & source) {
    if (scope == Scope::Project) return project_memory::add(category, content, tags, source);
    return personal_memory::add(category, content, tags, source);
}

}

void init()     {}
void shutdown() {}

size_t export_jsonl(Scope scope, std::string_view /*project_root*/,
                    std::string_view dest_path) {
    auto snaps = snapshot(scope);
    fs::path p{std::string(dest_path)};
    fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("export_jsonl: cannot write " + p.string());
    for (const auto & s : snaps) out << record_json(s).dump() << "\n";
    return snaps.size();
}

size_t export_markdown_bundle(Scope scope, std::string_view /*project_root*/,
                              std::string_view dest_dir) {
    auto snaps = snapshot(scope);
    fs::path dir{std::string(dest_dir)};
    fs::create_directories(dir);

    nlohmann::json manifest;
    manifest["count"]   = snaps.size();
    manifest["records"] = nlohmann::json::array();

    for (const auto & s : snaps) {
        char fname[64];
        std::snprintf(fname, sizeof(fname), "entry-%08lld.md", (long long)s.id);
        fs::path p = dir / fname;
        std::ofstream out(p, std::ios::binary | std::ios::trunc);
        if (!out) throw std::runtime_error("bundle: cannot write " + p.string());
        out << "# #" << s.id << " (" << s.category << ", " << iso_date(s.ts) << ")\n\n";
        if (!s.tags.empty()) {
            out << "Tags:";
            for (const auto & t : s.tags) out << " `" << t << "`";
            out << "\n\n";
        }
        if (!s.source.empty()) out << "Source: " << s.source << "\n\n";
        out << s.content << "\n";

        nlohmann::json rec = record_json(s);
        rec["filename"] = fname;
        manifest["records"].push_back(rec);
    }
    {
        std::ofstream out(dir / "index.json", std::ios::binary | std::ios::trunc);
        out << manifest.dump(2);
    }
    return snaps.size();
}

Counts import_jsonl(Scope scope, std::string_view /*project_root*/,
                    std::string_view src_path) {
    Counts c;
    fs::path p{std::string(src_path)};
    std::ifstream in(p);
    if (!in) throw std::runtime_error("import_jsonl: cannot read " + p.string());
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) { continue; }
        try {
            auto j = nlohmann::json::parse(line);
            std::string cat     = j.value("category", "");
            std::string content = j.value("content",  "");
            if (cat.empty() || content.empty()) { ++c.skipped; continue; }
            std::vector<std::string> tags;
            if (j.contains("tags") && j["tags"].is_array()) tags = j["tags"].get<std::vector<std::string>>();
            std::string orig_src = j.value("source", "");
            int64_t     orig_ts  = j.value("ts", (int64_t)0);
            add_to_store(scope, cat, content, tags, mark_origin(orig_src, orig_ts));
            ++c.entries;
        } catch (...) {
            ++c.skipped;
        }
    }
    return c;
}

Counts import_markdown_bundle(Scope scope, std::string_view /*project_root*/,
                              std::string_view src_dir) {
    Counts c;
    fs::path dir{std::string(src_dir)};
    fs::path index = dir / "index.json";
    if (!fs::exists(index))
        throw std::runtime_error("import_markdown_bundle: missing index.json in " + dir.string());
    std::ifstream in(index);
    nlohmann::json j; in >> j;
    if (!j.contains("records") || !j["records"].is_array()) return c;
    for (const auto & rec : j["records"]) {
        try {
            std::string cat     = rec.value("category", "");
            std::string content = rec.value("content",  "");
            if (cat.empty() || content.empty()) { ++c.skipped; continue; }
            std::vector<std::string> tags;
            if (rec.contains("tags") && rec["tags"].is_array()) tags = rec["tags"].get<std::vector<std::string>>();
            std::string orig_src = rec.value("source", "");
            int64_t     orig_ts  = rec.value("ts", (int64_t)0);
            add_to_store(scope, cat, content, tags, mark_origin(orig_src, orig_ts));
            ++c.entries;
        } catch (...) {
            ++c.skipped;
        }
    }
    return c;
}

}
