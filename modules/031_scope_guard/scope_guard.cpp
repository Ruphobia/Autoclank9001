#include "scope_guard.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace scope_guard {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;

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
fs::path sidecar_path(Scope scope, std::string_view project_root) {
    return store_dir(scope, project_root) / "scope_labels.json";
}

nlohmann::json load_doc(const fs::path & p) {
    if (!fs::exists(p)) return nlohmann::json{{"labels", nlohmann::json::object()}};
    std::ifstream in(p);
    try {
        nlohmann::json j; in >> j;
        if (!j.contains("labels") || !j["labels"].is_object()) j["labels"] = nlohmann::json::object();
        return j;
    } catch (...) {
        return nlohmann::json{{"labels", nlohmann::json::object()}};
    }
}
void save_doc(const fs::path & p, const nlohmann::json & j) {
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    if (!o) throw std::runtime_error("scope_guard: cannot write " + p.string());
    o << j.dump(2);
}

std::string key(int64_t id) { return std::to_string(id); }

bool allowed_for(Label l, Destination d, std::string * why) {
    if (l == Label::Redacted) {
        if (why) *why = "label=redacted; cannot leave the local store";
        return false;
    }
    switch (d) {
        case Destination::Display:      return true;
        case Destination::ModelContext: return true;
        case Destination::Sync:
            if (l == Label::ProjectShared) return true;
            if (why) *why = std::string("label=") + to_string(l) +
                            "; sync requires project-shared";
            return false;
        case Destination::Share:
            if (why) *why = std::string("label=") + to_string(l) +
                            "; explicit user export needed for sharing";
            return false;
    }
    return false;
}

}

const char * to_string(Label l) {
    switch (l) {
        case Label::LocalOnly:        return "local-only";
        case Label::ProjectShared:    return "project-shared";
        case Label::AssistantVisible: return "assistant-visible";
        case Label::Redacted:         return "redacted";
    }
    return "assistant-visible";
}
const char * to_string(Destination d) {
    switch (d) {
        case Destination::Sync:         return "sync";
        case Destination::Share:        return "share";
        case Destination::ModelContext: return "model-context";
        case Destination::Display:      return "display";
    }
    return "display";
}
Label label_from_string(std::string_view s) {
    if (s == "local-only")        return Label::LocalOnly;
    if (s == "project-shared")    return Label::ProjectShared;
    if (s == "assistant-visible") return Label::AssistantVisible;
    if (s == "redacted")          return Label::Redacted;
    return Label::AssistantVisible;
}

void init()     {}
void shutdown() {}

void set_label(Scope scope, int64_t id, Label l, std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path p = sidecar_path(scope, project_root);
    auto doc = load_doc(p);
    doc["labels"][key(id)] = to_string(l);
    save_doc(p, doc);
}

Label get_label(Scope scope, int64_t id, std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    auto doc = load_doc(sidecar_path(scope, project_root));
    if (!doc["labels"].contains(key(id))) return Label::AssistantVisible;
    return label_from_string(doc["labels"][key(id)].get<std::string>());
}

Verdict preflight(Scope scope, const std::vector<int64_t> & ids,
                  Destination dest, std::string_view project_root) {
    Verdict v;
    for (int64_t id : ids) {
        Label l = get_label(scope, id, project_root);
        Decision d;
        d.id      = id;
        d.label   = l;
        std::string why;
        d.allowed = allowed_for(l, dest, &why);
        d.reason  = d.allowed ? std::string{} : why;
        if (d.allowed) v.allowed.push_back(std::move(d));
        else           v.blocked.push_back(std::move(d));
    }
    return v;
}

}
