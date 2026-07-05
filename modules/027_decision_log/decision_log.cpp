#include "decision_log.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace decision_log {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;

fs::path log_dir(std::string_view project_root) {
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    return root / ".tool" / "decision-log";
}

fs::path log_path(std::string_view project_root) {
    return log_dir(project_root) / "decisions.jsonl";
}

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

int64_t next_id(std::string_view project_root) {
    fs::path p = log_path(project_root);
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

nlohmann::json rec_to_json(const Record & r) {
    nlohmann::json j;
    j["id"]              = r.id;
    j["ts"]              = r.ts;
    j["kind"]            = to_string(r.kind);
    j["parent_id"]       = r.parent_id;
    j["title"]           = r.title;
    j["context"]         = r.context;
    j["options"]         = r.options;
    j["chosen"]          = r.chosen;
    j["consequences"]    = r.consequences;
    j["linked_entries"]  = r.linked_entries;
    j["linked_code"]     = r.linked_code;
    j["note"]            = r.note;
    return j;
}

Record rec_from_json(const nlohmann::json & j) {
    Record r;
    r.id           = j.value("id",        (int64_t)0);
    r.ts           = j.value("ts",        (int64_t)0);
    std::string k  = j.value("kind",      "decision");
    if      (k == "amendment") r.kind = RecordKind::Amendment;
    else if (k == "reverse")   r.kind = RecordKind::Reverse;
    else                       r.kind = RecordKind::Decision;
    r.parent_id    = j.value("parent_id", (int64_t)0);
    r.title        = j.value("title",        "");
    r.context      = j.value("context",      "");
    if (j.contains("options"))         r.options        = j["options"].get<std::vector<std::string>>();
    r.chosen       = j.value("chosen",       "");
    r.consequences = j.value("consequences", "");
    if (j.contains("linked_entries"))  r.linked_entries = j["linked_entries"].get<std::vector<int64_t>>();
    if (j.contains("linked_code"))     r.linked_code    = j["linked_code"].get<std::vector<std::string>>();
    r.note         = j.value("note",         "");
    return r;
}

void append_record(const Record & r, std::string_view project_root) {
    fs::path p = log_path(project_root);
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::app);
    if (!o) throw std::runtime_error("decision_log: cannot append " + p.string());
    o << rec_to_json(r).dump() << "\n";
}

}

const char * to_string(RecordKind k) {
    switch (k) {
        case RecordKind::Decision:  return "decision";
        case RecordKind::Amendment: return "amendment";
        case RecordKind::Reverse:   return "reverse";
    }
    return "decision";
}

void init()     {}
void shutdown() {}

int64_t record(std::string_view              title,
               std::string_view              context,
               const std::vector<std::string> & options,
               std::string_view              chosen,
               std::string_view              consequences,
               const std::vector<int64_t>    & linked_entries,
               const std::vector<std::string> & linked_code,
               std::string_view              project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    Record r;
    r.id             = next_id(project_root);
    r.ts             = now_unix();
    r.kind           = RecordKind::Decision;
    r.title          = title;
    r.context        = context;
    r.options        = options;
    r.chosen         = chosen;
    r.consequences   = consequences;
    r.linked_entries = linked_entries;
    r.linked_code    = linked_code;
    append_record(r, project_root);
    return r.id;
}

int64_t amend(int64_t decision_id,
              const std::vector<int64_t>    & added_entries,
              const std::vector<std::string> & added_code,
              std::string_view              note,
              std::string_view              project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!get(decision_id, project_root))
        throw std::runtime_error("decision_log::amend: no such decision");
    Record r;
    r.id             = next_id(project_root);
    r.ts             = now_unix();
    r.kind           = RecordKind::Amendment;
    r.parent_id      = decision_id;
    r.linked_entries = added_entries;
    r.linked_code    = added_code;
    r.note           = note;
    append_record(r, project_root);
    return r.id;
}

int64_t reverse(int64_t decision_id, std::string_view note,
                std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!get(decision_id, project_root))
        throw std::runtime_error("decision_log::reverse: no such decision");
    Record r;
    r.id        = next_id(project_root);
    r.ts        = now_unix();
    r.kind      = RecordKind::Reverse;
    r.parent_id = decision_id;
    r.note      = note;
    append_record(r, project_root);
    return r.id;
}

std::vector<Record> all(std::string_view project_root) {
    std::vector<Record> out;
    fs::path p = log_path(project_root);
    if (!fs::exists(p)) return out;
    std::ifstream in(p);
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try {
            out.push_back(rec_from_json(nlohmann::json::parse(line)));
        } catch (...) {}
    }
    return out;
}

std::optional<Record> get(int64_t id, std::string_view project_root) {
    fs::path p = log_path(project_root);
    if (!fs::exists(p)) return std::nullopt;
    std::ifstream in(p);
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try {
            auto j = nlohmann::json::parse(line);
            if ((int64_t)j.value("id", 0) == id) return rec_from_json(j);
        } catch (...) {}
    }
    return std::nullopt;
}

std::optional<History> history(int64_t decision_id, std::string_view project_root) {
    auto base = get(decision_id, project_root);
    if (!base || base->kind != RecordKind::Decision) return std::nullopt;
    History h;
    h.decision = *base;
    for (const auto & r : all(project_root)) {
        if (r.parent_id != decision_id) continue;
        if      (r.kind == RecordKind::Amendment) h.amendments.push_back(r);
        else if (r.kind == RecordKind::Reverse)   h.reversals .push_back(r);
    }
    return h;
}

}
