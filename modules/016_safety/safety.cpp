#include "safety.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>

namespace safety {
namespace {

namespace fs = std::filesystem;

std::mutex                 g_mu;
std::vector<HazardEntry>   g_haz;
std::vector<LegalEntry>    g_leg;
bool                       g_loaded = false;

std::string to_lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

bool contains_ci(const std::string & haystack_lower, const std::string & needle) {
    if (needle.empty()) return false;
    std::string n = to_lower(needle);
    return haystack_lower.find(n) != std::string::npos;
}

fs::path bundled_dir() {
    // resources/ lives alongside the source root; the binary is built
    // into the source root so resources are next to argv[0]. We look
    // there first, then fall back to the current working directory so
    // out-of-tree invocations still find the seed.
    fs::path here = fs::current_path();
    if (fs::exists(here / "resources" / "safety" / "hazards.json")) return here / "resources" / "safety";
    return here / "resources" / "safety";
}

HazardEntry hazard_from_json(const nlohmann::json & j) {
    HazardEntry h;
    h.id               = j.value("id", "");
    h.material         = j.value("material", "");
    if (j.contains("synonyms"))         h.synonyms         = j["synonyms"].get<std::vector<std::string>>();
    if (j.contains("process_keywords")) h.process_keywords = j["process_keywords"].get<std::vector<std::string>>();
    h.mechanism        = j.value("mechanism", "");
    h.first_aid        = j.value("first_aid", "");
    if (j.contains("ppe"))              h.ppe              = j["ppe"].get<std::vector<std::string>>();
    h.severity         = j.value("severity", "");
    if (j.contains("sources"))          h.sources          = j["sources"].get<std::vector<std::string>>();
    return h;
}

LegalEntry legal_from_json(const nlohmann::json & j) {
    LegalEntry l;
    l.id              = j.value("id", "");
    l.jurisdiction    = j.value("jurisdiction", "");
    if (j.contains("topic_keywords")) l.topic_keywords = j["topic_keywords"].get<std::vector<std::string>>();
    l.cite            = j.value("cite", "");
    l.summary         = j.value("summary", "");
    return l;
}

void load_one(const fs::path & p, std::vector<HazardEntry> & haz,
              std::vector<LegalEntry> & leg, bool is_legal) {
    if (!fs::exists(p)) return;
    std::ifstream in(p);
    nlohmann::json j; in >> j;
    if (!j.contains("entries") || !j["entries"].is_array()) return;
    if (is_legal) {
        for (const auto & e : j["entries"]) leg.push_back(legal_from_json(e));
    } else {
        for (const auto & e : j["entries"]) haz.push_back(hazard_from_json(e));
    }
}

template <typename T>
void overlay_by_id(std::vector<T> & base, const std::vector<T> & overlay) {
    std::unordered_map<std::string, size_t> ix;
    for (size_t i = 0; i < base.size(); ++i) ix[base[i].id] = i;
    for (const auto & e : overlay) {
        auto it = ix.find(e.id);
        if (it == ix.end()) base.push_back(e);
        else                base[it->second] = e;
    }
}

}

void init(std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    g_haz.clear();
    g_leg.clear();
    g_loaded = false;

    fs::path seed = bundled_dir();
    load_one(seed / "hazards.json", g_haz, g_leg, false);
    load_one(seed / "legal.json",   g_haz, g_leg, true);

    if (!project_root.empty()) {
        fs::path overlay = fs::path(std::string(project_root)) / ".tool" / "safety";
        std::vector<HazardEntry> oh;
        std::vector<LegalEntry>  ol;
        load_one(overlay / "local_hazards.json", oh, ol, false);
        load_one(overlay / "local_legal.json",   oh, ol, true);
        overlay_by_id(g_haz, oh);
        overlay_by_id(g_leg, ol);
    }
    g_loaded = true;
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mu);
    g_haz.clear(); g_leg.clear(); g_loaded = false;
}

const std::vector<HazardEntry> & hazards() { return g_haz; }
const std::vector<LegalEntry>  & legal()   { return g_leg; }

Advice evaluate(std::string_view text) {
    std::lock_guard<std::mutex> lk(g_mu);
    Advice a;
    if (!g_loaded || text.empty()) return a;
    std::string hay = to_lower(text);

    std::unordered_set<std::string> seen_haz, seen_leg;
    for (const auto & h : g_haz) {
        if (seen_haz.count(h.id)) continue;
        bool hit = contains_ci(hay, h.material);
        if (!hit) for (const auto & s : h.synonyms)         if (contains_ci(hay, s)) { hit = true; break; }
        if (!hit) for (const auto & k : h.process_keywords) if (contains_ci(hay, k)) { hit = true; break; }
        if (hit) { a.hazards.push_back(h); seen_haz.insert(h.id); }
    }
    for (const auto & l : g_leg) {
        if (seen_leg.count(l.id)) continue;
        bool hit = false;
        for (const auto & k : l.topic_keywords) if (contains_ci(hay, k)) { hit = true; break; }
        if (hit) { a.legal.push_back(l); seen_leg.insert(l.id); }
    }
    a.preface = format_preface(a);
    return a;
}

std::string format_preface(const Advice & a) {
    if (a.hazards.empty() && a.legal.empty()) return {};
    std::ostringstream os;
    if (!a.hazards.empty()) {
        os << "**Hazards.**\n\n";
        for (const auto & h : a.hazards) {
            os << "- *" << h.material << "* (" << h.severity << "): "
               << h.mechanism;
            if (!h.first_aid.empty()) os << " First aid: " << h.first_aid;
            if (!h.ppe.empty()) {
                os << " PPE: ";
                for (size_t i = 0; i < h.ppe.size(); ++i) {
                    if (i) os << ", ";
                    os << h.ppe[i];
                }
                os << ".";
            }
            os << "\n";
        }
        os << "\n";
    }
    if (!a.legal.empty()) {
        os << "**Legal pointers** (not legal advice; verify against current statute):\n\n";
        for (const auto & l : a.legal) {
            os << "- *" << l.jurisdiction << "* (" << l.cite << "): " << l.summary << "\n";
        }
        os << "\n";
    }
    return os.str();
}

}
