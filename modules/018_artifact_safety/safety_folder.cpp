#include "safety_folder.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace safety_folder {
namespace {

namespace fs = std::filesystem;

std::string sanitize_id(std::string_view raw) {
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

fs::path incidents_dir(std::string_view project_root) {
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    return root / ".tool" / "safety" / "incidents";
}

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

std::string today_iso() {
    std::time_t t = std::time(nullptr);
    std::tm     tm{};
    ::gmtime_r(&t, &tm);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

std::string read_file(const fs::path & p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) return {};
    std::ostringstream b; b << in.rdbuf();
    return b.str();
}

void write_file(const fs::path & p, const std::string & body) {
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    if (!o) throw std::runtime_error("safety_folder: cannot write " + p.string());
    o << body;
}

size_t count_bullets_in_section(const std::string & text, const std::string & header) {
    auto pos = text.find(header + "\n");
    if (pos == std::string::npos) return 0;
    auto end = text.find("\n## ", pos + 1);
    std::string seg = text.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
    size_t count = 0;
    std::istringstream is(seg);
    std::string line;
    while (std::getline(is, line)) if (!line.empty() && line[0] == '-') ++count;
    return count;
}

std::string append_section_bullet(const std::string & body,
                                  const std::string & header,
                                  const std::string & bullet) {
    std::string out = body;
    if (!out.empty() && out.back() != '\n') out.push_back('\n');
    auto pos = out.find(header + "\n");
    if (pos == std::string::npos) {
        if (!out.empty() && out.back() != '\n') out.push_back('\n');
        out += "\n" + header + "\n\n" + bullet + "\n";
        return out;
    }
    // Insert at the end of the section (before the next "## " or EOF).
    auto next = out.find("\n## ", pos + 1);
    if (next == std::string::npos) {
        if (out.back() != '\n') out.push_back('\n');
        out += bullet + "\n";
        return out;
    }
    std::string before = out.substr(0, next);
    std::string after  = out.substr(next);
    if (!before.empty() && before.back() != '\n') before.push_back('\n');
    before += bullet + "\n";
    return before + after;
}

fs::path entry_path(std::string_view id, std::string_view project_root) {
    return incidents_dir(project_root) / (sanitize_id(id) + ".md");
}

}

void init()     {}
void shutdown() {}

bool has_been_explained(std::string_view hazard_id, std::string_view project_root) {
    auto p = entry_path(hazard_id, project_root);
    return fs::exists(p);
}

void record_hazard(std::string_view hazard_id,
                   std::string_view title,
                   std::string_view mechanism,
                   std::string_view project_root) {
    std::string id = sanitize_id(hazard_id);
    if (id.empty()) throw std::runtime_error("safety_folder::record_hazard: empty id");
    auto p = entry_path(id, project_root);

    std::string body;
    if (fs::exists(p)) {
        body = read_file(p);
    } else {
        std::ostringstream os;
        os << "# " << title << " (`" << id << "`)\n\n"
           << "First seen: " << today_iso() << "\n\n"
           << "## Mechanism\n\n" << mechanism << "\n\n"
           << "## Mitigations\n\n"
           << "## Incidents\n";
        body = os.str();
    }
    // Update the "Last touched" line idempotently (kept on its own line
    // near the top so it does not churn user-edited body content).
    static const std::regex re("\nLast touched: [^\n]*\n");
    std::string stamp = "\nLast touched: " + today_iso() + "\n";
    if (std::regex_search(body, re)) {
        body = std::regex_replace(body, re, stamp);
    } else {
        auto pos = body.find("First seen:");
        if (pos != std::string::npos) {
            auto eol = body.find('\n', pos);
            if (eol != std::string::npos) {
                body.insert(eol + 1, stamp.substr(1));     // drop leading \n
            }
        } else {
            body = "Last touched: " + today_iso() + "\n\n" + body;
        }
    }
    write_file(p, body);
}

void log_mitigation(std::string_view hazard_id, std::string_view note,
                    std::string_view project_root) {
    std::string id = sanitize_id(hazard_id);
    if (id.empty()) throw std::runtime_error("safety_folder::log_mitigation: empty id");
    if (!has_been_explained(id, project_root)) {
        record_hazard(id, id, "(no mechanism on record yet)", project_root);
    }
    auto p = entry_path(id, project_root);
    std::string body = read_file(p);
    std::string bullet = "- " + today_iso() + ": " + std::string(note);
    body = append_section_bullet(body, "## Mitigations", bullet);
    write_file(p, body);
}

void log_incident(std::string_view hazard_id, std::string_view note,
                  std::string_view project_root) {
    std::string id = sanitize_id(hazard_id);
    if (id.empty()) throw std::runtime_error("safety_folder::log_incident: empty id");
    if (!has_been_explained(id, project_root)) {
        record_hazard(id, id, "(no mechanism on record yet)", project_root);
    }
    auto p = entry_path(id, project_root);
    std::string body = read_file(p);
    std::string bullet = "- " + today_iso() + ": " + std::string(note);
    body = append_section_bullet(body, "## Incidents", bullet);
    write_file(p, body);
}

std::vector<std::string> list_known(std::string_view project_root) {
    std::vector<std::string> out;
    fs::path dir = incidents_dir(project_root);
    if (!fs::exists(dir)) return out;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        if (e.path().extension() == ".md") out.push_back(e.path().stem().string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

std::optional<Entry> read(std::string_view hazard_id, std::string_view project_root) {
    auto p = entry_path(hazard_id, project_root);
    if (!fs::exists(p)) return std::nullopt;
    Entry e;
    e.id   = sanitize_id(hazard_id);
    e.body = read_file(p);

    // Extract the H1 as the title (everything after "# ").
    auto pos = e.body.find("# ");
    if (pos == 0 || (pos != std::string::npos && (pos == 0 || e.body[pos - 1] == '\n'))) {
        auto eol = e.body.find('\n', pos);
        e.title = e.body.substr(pos + 2, eol == std::string::npos ? std::string::npos : eol - pos - 2);
    }
    e.mitigation_count = count_bullets_in_section(e.body, "## Mitigations");
    e.incident_count   = count_bullets_in_section(e.body, "## Incidents");
    e.first_seen_unix = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            fs::last_write_time(p).time_since_epoch()).count());
    e.last_touched_unix = now_unix();
    return e;
}

}
