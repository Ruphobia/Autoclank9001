#include "legal_folder.hpp"

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace legal_folder {
namespace {

namespace fs = std::filesystem;

std::string sanitize_id(std::string_view raw) {
    std::string out;
    out.reserve(raw.size());
    for (char c : raw) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-') out.push_back(c);
    }
    return out;
}

fs::path pathways_dir(std::string_view project_root) {
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    return root / ".tool" / "legal" / "pathways";
}

fs::path entry_path(std::string_view id, std::string_view project_root) {
    return pathways_dir(project_root) / (sanitize_id(id) + ".md");
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
    if (!o) throw std::runtime_error("legal_folder: cannot write " + p.string());
    o << body;
}

const char * section_for(EventKind k) {
    switch (k) {
        case EventKind::Status:   return "## Status";
        case EventKind::Followup: return "## Followups";
        case EventKind::Contact:  return "## Contacts";
        case EventKind::Fee:      return "## Fees";
        case EventKind::Note:     return "## Notes";
    }
    return "## Notes";
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

std::string append_bullet(const std::string & body,
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

void touch_last(std::string & body) {
    static const std::regex re("\nLast touched: [^\n]*\n");
    std::string stamp = "\nLast touched: " + today_iso() + "\n";
    if (std::regex_search(body, re)) {
        body = std::regex_replace(body, re, stamp);
        return;
    }
    auto pos = body.find("Jurisdiction:");
    if (pos == std::string::npos) {
        body = "Last touched: " + today_iso() + "\n\n" + body;
        return;
    }
    auto eol = body.find('\n', pos);
    if (eol != std::string::npos) body.insert(eol + 1, stamp.substr(1));
}

}

const char * to_string(EventKind k) {
    switch (k) {
        case EventKind::Status:   return "status";
        case EventKind::Followup: return "followup";
        case EventKind::Contact:  return "contact";
        case EventKind::Fee:      return "fee";
        case EventKind::Note:     return "note";
    }
    return "note";
}

void init()     {}
void shutdown() {}

bool has_been_explained(std::string_view id, std::string_view project_root) {
    return fs::exists(entry_path(id, project_root));
}

void record_pathway(std::string_view id,
                    std::string_view jurisdiction,
                    std::string_view cite,
                    std::string_view summary,
                    std::string_view project_root) {
    std::string sid = sanitize_id(id);
    if (sid.empty()) throw std::runtime_error("legal_folder::record_pathway: empty id");
    auto p = entry_path(sid, project_root);
    std::string body;
    if (fs::exists(p)) {
        body = read_file(p);
    } else {
        std::ostringstream os;
        os << "# " << id << " (`" << sid << "`)\n\n"
           << "Jurisdiction: " << jurisdiction << "\n"
           << "Citation: " << cite << "\n"
           << "First seen: " << today_iso() << "\n\n"
           << "## Summary\n\n" << summary << "\n\n"
           << "## Status\n\n"
           << "## Followups\n\n"
           << "## Contacts\n\n"
           << "## Fees\n\n"
           << "## Notes\n";
        body = os.str();
    }
    touch_last(body);
    write_file(p, body);
}

void log_event(std::string_view id, EventKind kind, std::string_view note,
               std::string_view project_root) {
    std::string sid = sanitize_id(id);
    if (sid.empty()) throw std::runtime_error("legal_folder::log_event: empty id");
    if (!has_been_explained(sid, project_root)) {
        record_pathway(sid, "(unknown)", "(unknown)",
                       "(no summary on record yet)", project_root);
    }
    auto p = entry_path(sid, project_root);
    std::string body = read_file(p);
    std::string bullet = "- " + today_iso() + " [" + to_string(kind) + "] " + std::string(note);
    body = append_bullet(body, section_for(kind), bullet);
    write_file(p, body);
}

std::vector<std::string> list_known(std::string_view project_root) {
    std::vector<std::string> out;
    fs::path dir = pathways_dir(project_root);
    if (!fs::exists(dir)) return out;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        if (e.path().extension() == ".md") out.push_back(e.path().stem().string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

std::optional<Entry> read(std::string_view id, std::string_view project_root) {
    auto p = entry_path(id, project_root);
    if (!fs::exists(p)) return std::nullopt;
    Entry e;
    e.id   = sanitize_id(id);
    e.body = read_file(p);

    auto find_field = [&](const std::string & label) {
        auto pos = e.body.find(label + ": ");
        if (pos == std::string::npos) return std::string{};
        auto eol = e.body.find('\n', pos);
        return e.body.substr(pos + label.size() + 2,
                             eol == std::string::npos ? std::string::npos : eol - (pos + label.size() + 2));
    };
    e.jurisdiction = find_field("Jurisdiction");
    e.cite         = find_field("Citation");
    e.status_count   = count_bullets_in_section(e.body, "## Status");
    e.followup_count = count_bullets_in_section(e.body, "## Followups");
    e.contact_count  = count_bullets_in_section(e.body, "## Contacts");
    e.fee_count      = count_bullets_in_section(e.body, "## Fees");
    return e;
}

}
