#include "decisions_folder.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace decisions_folder {
namespace {

namespace fs = std::filesystem;

std::string sanitize_slug(std::string_view raw) {
    std::string out;
    out.reserve(raw.size());
    for (char c : raw) {
        char lc = (char)std::tolower((unsigned char)c);
        if ((lc >= 'a' && lc <= 'z') || (lc >= '0' && lc <= '9')) out.push_back(lc);
        else if (lc == ' ' || lc == '_' || lc == '-') out.push_back('-');
    }
    // Collapse runs of '-'.
    std::string collapsed;
    bool last_dash = false;
    for (char c : out) {
        if (c == '-') {
            if (!last_dash && !collapsed.empty()) collapsed.push_back('-');
            last_dash = true;
        } else {
            collapsed.push_back(c);
            last_dash = false;
        }
    }
    while (!collapsed.empty() && collapsed.back() == '-') collapsed.pop_back();
    return collapsed;
}

fs::path decisions_dir(std::string_view project_root) {
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    return root / ".tool" / "decisions";
}

std::string today_iso() {
    std::time_t t = std::time(nullptr);
    std::tm tm{}; ::gmtime_r(&t, &tm);
    char buf[32]; std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

std::string padded(int n) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%04d", n);
    return buf;
}

fs::path find_path_for(int number, std::string_view project_root) {
    fs::path dir = decisions_dir(project_root);
    if (!fs::exists(dir)) return {};
    std::string prefix = padded(number) + "-";
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        std::string name = e.path().filename().string();
        if (name.rfind(prefix, 0) == 0 && e.path().extension() == ".md") return e.path();
    }
    return {};
}

int next_ordinal(std::string_view project_root) {
    fs::path dir = decisions_dir(project_root);
    int max_n = 0;
    if (!fs::exists(dir)) return 1;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file() || e.path().extension() != ".md") continue;
        std::string name = e.path().filename().string();
        if (name.size() < 5) continue;
        try {
            int n = std::stoi(name.substr(0, 4));
            if (n > max_n) max_n = n;
        } catch (...) {}
    }
    return max_n + 1;
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
    if (!o) throw std::runtime_error("decisions_folder: cannot write " + p.string());
    o << body;
}

std::string set_status_line(std::string body, Status s) {
    static const std::regex re("Status: [^\n]*");
    std::string repl = std::string("Status: ") + to_string(s);
    if (std::regex_search(body, re))
        body = std::regex_replace(body, re, repl, std::regex_constants::format_first_only);
    else
        body = repl + "\n" + body;
    return body;
}

std::string read_section(const std::string & body, const std::string & header) {
    auto pos = body.find(header + "\n");
    if (pos == std::string::npos) return {};
    pos = body.find('\n', pos);
    if (pos == std::string::npos) return {};
    auto end = body.find("\n## ", pos + 1);
    std::string s = body.substr(pos + 1, end == std::string::npos ? std::string::npos : end - (pos + 1));
    // Trim leading/trailing blank lines.
    while (!s.empty() && (s.front() == '\n' || s.front() == '\r' || s.front() == ' ')) s.erase(0, 1);
    while (!s.empty() && (s.back()  == '\n' || s.back()  == '\r' || s.back()  == ' ')) s.pop_back();
    return s;
}

std::string append_bullet_in_section(const std::string & body,
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

std::vector<int> parse_ordinals_from(const std::string & section) {
    std::vector<int> out;
    static const std::regex re("ADR-(\\d+)");
    auto begin = std::sregex_iterator(section.begin(), section.end(), re);
    auto end   = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        try { out.push_back(std::stoi((*it)[1].str())); } catch (...) {}
    }
    return out;
}

}

const char * to_string(Status s) {
    switch (s) {
        case Status::Proposed:   return "proposed";
        case Status::Accepted:   return "accepted";
        case Status::Deprecated: return "deprecated";
        case Status::Superseded: return "superseded";
    }
    return "proposed";
}
Status status_from_string(std::string_view s) {
    if (s == "proposed")   return Status::Proposed;
    if (s == "accepted")   return Status::Accepted;
    if (s == "deprecated") return Status::Deprecated;
    if (s == "superseded") return Status::Superseded;
    return Status::Proposed;
}

void init()     {}
void shutdown() {}

int record(std::string_view  slug,
           std::string_view  title,
           std::string_view  context,
           std::string_view  decision_text,
           std::string_view  consequences,
           std::string_view  project_root,
           Status            initial_status) {
    std::string ss = sanitize_slug(slug);
    if (ss.empty()) throw std::runtime_error("decisions_folder::record: empty slug");

    int n = next_ordinal(project_root);
    fs::path dir  = decisions_dir(project_root);
    fs::path path = dir / (padded(n) + "-" + ss + ".md");

    std::ostringstream os;
    os << "# ADR-" << padded(n) << ": " << title << "\n\n"
       << "Status: " << to_string(initial_status) << "\n"
       << "Date: "   << today_iso() << "\n\n"
       << "## Context\n\n"      << context      << "\n\n"
       << "## Decision\n\n"     << decision_text << "\n\n"
       << "## Consequences\n\n" << consequences << "\n";
    write_file(path, os.str());
    return n;
}

void set_status(int number, Status s, std::string_view project_root) {
    fs::path p = find_path_for(number, project_root);
    if (p.empty()) throw std::runtime_error("decisions_folder::set_status: ADR not found");
    std::string body = read_file(p);
    body = set_status_line(body, s);
    write_file(p, body);
}

void supersede(int newer, int older, std::string_view project_root) {
    fs::path np = find_path_for(newer, project_root);
    fs::path op = find_path_for(older, project_root);
    if (np.empty()) throw std::runtime_error("decisions_folder::supersede: newer ADR not found");
    if (op.empty()) throw std::runtime_error("decisions_folder::supersede: older ADR not found");

    std::string nb = read_file(np);
    nb = append_bullet_in_section(nb, "## Supersedes",
        "- ADR-" + padded(older) + " (" + today_iso() + ")");
    write_file(np, nb);

    std::string ob = read_file(op);
    ob = set_status_line(ob, Status::Superseded);
    ob = append_bullet_in_section(ob, "## Superseded by",
        "- ADR-" + padded(newer) + " (" + today_iso() + ")");
    write_file(op, ob);
}

std::vector<int> list(std::string_view project_root) {
    std::vector<int> out;
    fs::path dir = decisions_dir(project_root);
    if (!fs::exists(dir)) return out;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file() || e.path().extension() != ".md") continue;
        std::string name = e.path().filename().string();
        try { out.push_back(std::stoi(name.substr(0, 4))); } catch (...) {}
    }
    std::sort(out.begin(), out.end());
    return out;
}

std::optional<Decision> read(int number, std::string_view project_root) {
    fs::path p = find_path_for(number, project_root);
    if (p.empty()) return std::nullopt;
    Decision d;
    d.number = number;
    d.body   = read_file(p);
    std::string fn = p.filename().string();
    if (fn.size() > 5) d.slug = fn.substr(5, fn.size() - 5 - 3);   // strip 0001- and .md

    auto h1 = d.body.find("# ");
    if (h1 != std::string::npos) {
        auto eol = d.body.find('\n', h1);
        d.title = d.body.substr(h1 + 2, eol == std::string::npos ? std::string::npos : eol - h1 - 2);
    }
    auto find_field = [&](const std::string & label) {
        auto pos = d.body.find(label + ": ");
        if (pos == std::string::npos) return std::string{};
        auto eol = d.body.find('\n', pos);
        return d.body.substr(pos + label.size() + 2,
                             eol == std::string::npos ? std::string::npos : eol - (pos + label.size() + 2));
    };
    d.status       = status_from_string(find_field("Status"));
    d.context      = read_section(d.body, "## Context");
    d.decision     = read_section(d.body, "## Decision");
    d.consequences = read_section(d.body, "## Consequences");
    d.supersedes    = parse_ordinals_from(read_section(d.body, "## Supersedes"));
    d.superseded_by = parse_ordinals_from(read_section(d.body, "## Superseded by"));
    return d;
}

}
