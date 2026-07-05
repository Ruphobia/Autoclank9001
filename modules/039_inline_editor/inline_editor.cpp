#include "inline_editor.hpp"

#include "../022_project_memory/project_memory.hpp"
#include "../023_personal_memory/personal_memory.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace inline_editor {
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
fs::path history_path(Scope scope, int64_t id, std::string_view project_root) {
    fs::path dir = store_dir(scope, project_root) / "edit_history";
    fs::create_directories(dir);
    return dir / (std::to_string(id) + ".jsonl");
}

nlohmann::json snap_to_json(const Snapshot & s) {
    return nlohmann::json{
        {"revision", s.revision},
        {"ts",       s.ts},
        {"content",  s.content},
        {"tags",     s.tags},
        {"source",   s.source},
    };
}
Snapshot snap_from_json(const nlohmann::json & j) {
    Snapshot s;
    s.revision = j.value("revision", (int64_t)0);
    s.ts       = j.value("ts",       (int64_t)0);
    s.content  = j.value("content",  "");
    if (j.contains("tags") && j["tags"].is_array()) s.tags = j["tags"].get<std::vector<std::string>>();
    s.source   = j.value("source",   "");
    return s;
}

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

std::optional<Snapshot> current_snapshot(Scope scope, int64_t id, std::string_view project_root) {
    if (scope == Scope::Project) {
        auto e = project_memory::get(id);
        if (!e) return std::nullopt;
        Snapshot s;
        s.revision = -1;     // -1 = live
        s.ts       = e->ts;
        s.content  = e->content;
        s.tags     = e->tags;
        s.source   = e->source;
        return s;
    }
    auto e = personal_memory::get(id);
    if (!e) return std::nullopt;
    Snapshot s;
    s.revision = -1;
    s.ts       = e->ts;
    s.content  = e->content;
    s.tags     = e->tags;
    s.source   = e->source;
    return s;
}

}

void init()     {}
void shutdown() {}

int64_t edit(Scope scope, int64_t id,
             std::string_view new_content,
             const std::vector<std::string> & new_tags,
             std::string_view new_source,
             std::string_view project_root) {
    auto before = current_snapshot(scope, id, project_root);
    if (!before) throw std::runtime_error("inline_editor::edit: no such live entry");

    auto hpath = history_path(scope, id, project_root);
    auto existing = history(scope, id, project_root);
    int64_t rev = existing.empty() ? 1 : (existing.back().revision + 1);

    Snapshot snap = *before;
    snap.revision = rev;
    snap.ts       = now_unix();

    {
        std::ofstream o(hpath, std::ios::binary | std::ios::app);
        if (!o) throw std::runtime_error("inline_editor::edit: cannot append history");
        o << snap_to_json(snap).dump() << "\n";
    }

    if (scope == Scope::Project)
        project_memory::update(id, new_content, new_tags, new_source);
    else
        personal_memory::update(id, new_content, new_tags, new_source);
    return rev;
}

std::vector<Snapshot> history(Scope scope, int64_t id, std::string_view project_root) {
    std::vector<Snapshot> out;
    auto p = history_path(scope, id, project_root);
    if (!fs::exists(p)) return out;
    std::ifstream in(p);
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try { out.push_back(snap_from_json(nlohmann::json::parse(line))); } catch (...) {}
    }
    std::sort(out.begin(), out.end(),
              [](const Snapshot & a, const Snapshot & b){ return a.revision < b.revision; });
    return out;
}

std::vector<LineDiff> diff_revisions(Scope scope, int64_t id,
                                     int64_t rev_a, int64_t rev_b,
                                     std::string_view project_root) {
    auto pick = [&](int64_t rev) -> std::optional<Snapshot> {
        if (rev < 0) return current_snapshot(scope, id, project_root);
        for (auto & s : history(scope, id, project_root)) if (s.revision == rev) return s;
        return std::nullopt;
    };
    auto sa = pick(rev_a);
    auto sb = pick(rev_b);
    if (!sa || !sb) return {};

    auto la = split_lines(sa->content);
    auto lb = split_lines(sb->content);

    // Trivial line-diff: emit removed lines first, then added lines, then
    // identical leading/trailing prefix as Same. Good enough for the UI.
    std::vector<LineDiff> out;
    size_t pre = 0;
    while (pre < la.size() && pre < lb.size() && la[pre] == lb[pre]) {
        out.push_back({LineDiff::Kind::Same, la[pre]});
        ++pre;
    }
    for (size_t i = pre; i < la.size(); ++i) out.push_back({LineDiff::Kind::Removed, la[i]});
    for (size_t i = pre; i < lb.size(); ++i) out.push_back({LineDiff::Kind::Added,   lb[i]});
    return out;
}

std::vector<LinkRef> validate_links(std::string_view content, Scope scope,
                                    std::string_view project_root) {
    std::vector<LinkRef> out;
    static const std::regex re(R"(\[\[(\d+)\]\])");
    std::string s{content};
    for (auto it = std::sregex_iterator(s.begin(), s.end(), re);
         it != std::sregex_iterator(); ++it) {
        LinkRef r;
        r.raw       = (*it)[0].str();
        r.target_id = std::stoll((*it)[1].str());
        if (scope == Scope::Project) {
            r.exists = static_cast<bool>(project_memory::get(r.target_id));
        } else {
            r.exists = static_cast<bool>(personal_memory::get(r.target_id));
        }
        out.push_back(r);
    }
    return out;
}

}
