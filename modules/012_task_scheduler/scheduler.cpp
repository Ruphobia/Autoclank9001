#include "scheduler.hpp"

#include "../011_scripting/scripting.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace scheduler {
namespace {

namespace fs = std::filesystem;

constexpr size_t kMaxPersistedStream = 64 * 1024;   // 64 KiB per stream

std::string sanitize_name(std::string_view raw) {
    std::string out;
    out.reserve(raw.size());
    for (char c : raw) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-') {
            out.push_back(c);
        }
    }
    return out;
}

fs::path plans_dir(std::string_view project_root) {
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    return root / ".tool" / "plans";
}

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

std::string truncate_for_persist(const std::string & s) {
    if (s.size() <= kMaxPersistedStream) return s;
    return s.substr(0, kMaxPersistedStream) + "\n... [truncated]";
}

nlohmann::json task_to_json(const Task & t) {
    nlohmann::json j;
    j["id"]               = t.id;
    j["description"]      = t.description;
    j["body"]             = t.body;
    j["depends_on"]       = t.depends_on;
    j["timeout_seconds"]  = t.timeout_seconds;
    j["status"]           = to_string(t.status);
    j["exit_code"]        = t.exit_code;
    j["timed_out"]        = t.timed_out;
    j["stdout"]           = truncate_for_persist(t.stdout_text);
    j["stderr"]           = truncate_for_persist(t.stderr_text);
    j["started_at_unix"]  = t.started_at_unix;
    j["finished_at_unix"] = t.finished_at_unix;
    return j;
}

Task task_from_json(const nlohmann::json & j) {
    Task t;
    t.id              = j.value("id", "");
    t.description     = j.value("description", "");
    t.body            = j.value("body", "");
    if (j.contains("depends_on") && j["depends_on"].is_array())
        t.depends_on  = j["depends_on"].get<std::vector<std::string>>();
    t.timeout_seconds = j.value("timeout_seconds", 60);
    t.status          = status_from_string(j.value("status", "pending"));
    t.exit_code       = j.value("exit_code", 0);
    t.timed_out       = j.value("timed_out", false);
    t.stdout_text     = j.value("stdout", "");
    t.stderr_text     = j.value("stderr", "");
    t.started_at_unix = j.value("started_at_unix",  (int64_t)0);
    t.finished_at_unix= j.value("finished_at_unix", (int64_t)0);
    return t;
}

}

const char * to_string(Status s) {
    switch (s) {
        case Status::Pending: return "pending";
        case Status::Running: return "running";
        case Status::Done:    return "done";
        case Status::Failed:  return "failed";
        case Status::Skipped: return "skipped";
    }
    return "pending";
}

Status status_from_string(std::string_view s) {
    if (s == "pending") return Status::Pending;
    if (s == "running") return Status::Running;
    if (s == "done")    return Status::Done;
    if (s == "failed")  return Status::Failed;
    if (s == "skipped") return Status::Skipped;
    return Status::Pending;
}

void init()     { /* lazy directory creation; nothing else to do */ }
void shutdown() { /* nothing held */ }

void save_plan(const Plan & p, std::string_view project_root) {
    std::string name = sanitize_name(p.name);
    if (name.empty()) throw std::runtime_error("scheduler::save_plan: empty/invalid name");

    fs::path dir = plans_dir(project_root);
    fs::create_directories(dir);
    fs::path path = dir / (name + ".json");

    nlohmann::json j;
    j["name"]        = name;
    j["description"] = p.description;
    j["tasks"]       = nlohmann::json::array();
    for (const auto & t : p.tasks) j["tasks"].push_back(task_to_json(t));

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("scheduler::save_plan: cannot write " + path.string());
    out << j.dump(2);
}

Plan load_plan(std::string_view raw_name, std::string_view project_root) {
    std::string name = sanitize_name(raw_name);
    if (name.empty()) throw std::runtime_error("scheduler::load_plan: empty/invalid name");
    fs::path path = plans_dir(project_root) / (name + ".json");
    if (!fs::exists(path))
        throw std::runtime_error("scheduler::load_plan: no such plan: " + path.string());

    std::ifstream in(path);
    nlohmann::json j; in >> j;

    Plan p;
    p.name        = j.value("name", name);
    p.description = j.value("description", "");
    if (j.contains("tasks") && j["tasks"].is_array()) {
        for (const auto & tj : j["tasks"]) p.tasks.push_back(task_from_json(tj));
    }
    return p;
}

std::vector<std::string> list_plans(std::string_view project_root) {
    std::vector<std::string> names;
    fs::path dir = plans_dir(project_root);
    if (!fs::exists(dir)) return names;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        if (e.path().extension() == ".json") names.push_back(e.path().stem().string());
    }
    std::sort(names.begin(), names.end());
    return names;
}

std::optional<size_t> next_runnable(Plan & p) {
    std::unordered_map<std::string, size_t> by_id;
    for (size_t i = 0; i < p.tasks.size(); ++i) by_id[p.tasks[i].id] = i;

    for (size_t i = 0; i < p.tasks.size(); ++i) {
        Task & t = p.tasks[i];
        if (t.status != Status::Pending) continue;

        bool blocked = false;
        bool poisoned = false;
        for (const auto & dep : t.depends_on) {
            auto it = by_id.find(dep);
            if (it == by_id.end()) { poisoned = true; break; }
            Status s = p.tasks[it->second].status;
            if (s == Status::Failed || s == Status::Skipped) { poisoned = true; break; }
            if (s != Status::Done) { blocked = true; break; }
        }
        if (poisoned) {
            t.status           = Status::Skipped;
            t.finished_at_unix = now_unix();
            continue;
        }
        if (blocked) continue;
        return i;
    }
    return std::nullopt;
}

std::optional<size_t> run_step(Plan & p, std::string_view project_root,
                               std::string_view cwd) {
    auto idx = next_runnable(p);
    if (!idx) return std::nullopt;
    Task & t = p.tasks[*idx];

    t.status          = Status::Running;
    t.started_at_unix = now_unix();
    save_plan(p, project_root);

    auto r = scripting::execute(t.body, cwd, t.timeout_seconds);
    t.stdout_text     = r.stdout_text;
    t.stderr_text     = r.stderr_text;
    t.exit_code       = r.exit_code;
    t.timed_out       = r.timed_out;
    t.finished_at_unix = now_unix();
    t.status          = (r.exit_code == 0 && !r.timed_out) ? Status::Done : Status::Failed;
    save_plan(p, project_root);
    return idx;
}

size_t run_all(Plan & p, std::string_view project_root, std::string_view cwd) {
    size_t ran = 0;
    while (run_step(p, project_root, cwd)) ++ran;
    return ran;
}

}
