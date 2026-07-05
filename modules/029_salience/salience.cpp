#include "salience.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace salience {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;

int64_t now_unix_now() {
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

fs::path sidecar_path(Scope scope, std::string_view project_root) {
    return store_dir(scope, project_root) / "salience.json";
}

nlohmann::json load_doc(const fs::path & p) {
    if (!fs::exists(p)) return nlohmann::json{{"weights", nlohmann::json::object()}};
    std::ifstream in(p);
    try {
        nlohmann::json j; in >> j;
        if (!j.contains("weights") || !j["weights"].is_object()) j["weights"] = nlohmann::json::object();
        return j;
    } catch (...) {
        return nlohmann::json{{"weights", nlohmann::json::object()}};
    }
}

void save_doc(const fs::path & p, const nlohmann::json & j) {
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    if (!o) throw std::runtime_error("salience: cannot write " + p.string());
    o << j.dump(2);
}

std::string key(int64_t id) { return std::to_string(id); }

Weight weight_from_json(const nlohmann::json & j) {
    Weight w;
    w.base          = j.value("base", 1.0);
    w.decay_per_day = j.value("decay_per_day", 0.05);
    w.pinned        = j.value("pinned", false);
    w.touched_unix  = j.value("touched_unix", (int64_t)0);
    return w;
}
nlohmann::json json_from_weight(const Weight & w) {
    return nlohmann::json{
        {"base",          w.base},
        {"decay_per_day", w.decay_per_day},
        {"pinned",        w.pinned},
        {"touched_unix",  w.touched_unix},
    };
}

template <typename F>
void mutate(Scope scope, int64_t id, std::string_view project_root, F && f) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path p = sidecar_path(scope, project_root);
    auto doc = load_doc(p);
    Weight w;
    if (doc["weights"].contains(key(id))) w = weight_from_json(doc["weights"][key(id)]);
    f(w);
    doc["weights"][key(id)] = json_from_weight(w);
    save_doc(p, doc);
}

}

void init()     {}
void shutdown() {}

void set_weight(Scope scope, int64_t id, double base, std::string_view project_root) {
    if (base < 0.0) base = 0.0;
    mutate(scope, id, project_root, [&](Weight & w){
        w.base = base;
        if (w.touched_unix == 0) w.touched_unix = now_unix_now();
    });
}
void set_decay(Scope scope, int64_t id, double per_day, std::string_view project_root) {
    if (per_day < 0.0) per_day = 0.0;
    mutate(scope, id, project_root, [&](Weight & w){ w.decay_per_day = per_day; });
}
void pin(Scope scope, int64_t id, std::string_view project_root) {
    mutate(scope, id, project_root, [&](Weight & w){ w.pinned = true; });
}
void unpin(Scope scope, int64_t id, std::string_view project_root) {
    mutate(scope, id, project_root, [&](Weight & w){ w.pinned = false; });
}
void touch(Scope scope, int64_t id, int64_t when, std::string_view project_root) {
    int64_t ts = when ? when : now_unix_now();
    mutate(scope, id, project_root, [&](Weight & w){ w.touched_unix = ts; });
}

Weight weight(Scope scope, int64_t id, std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    auto doc = load_doc(sidecar_path(scope, project_root));
    if (doc["weights"].contains(key(id))) return weight_from_json(doc["weights"][key(id)]);
    return Weight{};
}

double effective_weight(Scope scope, int64_t id, int64_t now_unix,
                        std::string_view project_root) {
    Weight w = weight(scope, id, project_root);
    if (w.pinned) return w.base;
    if (w.touched_unix == 0) return w.base;
    double days = (now_unix - w.touched_unix) / 86400.0;
    if (days < 0) days = 0;
    return w.base * std::exp(-w.decay_per_day * days);
}

std::vector<int64_t> rerank(Scope scope, std::vector<int64_t> ids,
                            int64_t now_unix, std::string_view project_root) {
    std::vector<std::pair<int64_t,double>> scored;
    scored.reserve(ids.size());
    for (int64_t id : ids) scored.emplace_back(id, effective_weight(scope, id, now_unix, project_root));
    std::sort(scored.begin(), scored.end(),
              [](const auto & a, const auto & b){ return a.second > b.second; });
    std::vector<int64_t> out;
    out.reserve(scored.size());
    for (auto & p : scored) out.push_back(p.first);
    return out;
}

}
