#include "ip_folder.hpp"

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace ip_folder {
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

fs::path ip_dir(std::string_view project_root) {
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    return root / ".tool" / "ip";
}

fs::path entry_path(std::string_view id, std::string_view project_root) {
    return ip_dir(project_root) / (sanitize_id(id) + ".md");
}

std::string today_iso() {
    std::time_t t = std::time(nullptr);
    std::tm tm{}; ::gmtime_r(&t, &tm);
    char buf[32]; std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
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
    if (!o) throw std::runtime_error("ip_folder: cannot write " + p.string());
    o << body;
}

std::vector<std::string> bullets_in_section(const std::string & text, const std::string & header) {
    std::vector<std::string> out;
    auto pos = text.find(header + "\n");
    if (pos == std::string::npos) return out;
    auto end = text.find("\n## ", pos + 1);
    std::string seg = text.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
    std::istringstream is(seg);
    std::string line;
    while (std::getline(is, line)) {
        if (!line.empty() && line[0] == '-') {
            out.push_back(line.substr(line.find_first_not_of("- ")));
        }
    }
    return out;
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
    auto pos = body.find("Status:");
    if (pos == std::string::npos) {
        body = "Last touched: " + today_iso() + "\n\n" + body;
        return;
    }
    auto eol = body.find('\n', pos);
    if (eol != std::string::npos) body.insert(eol + 1, stamp.substr(1));
}

void set_status_field(std::string & body, std::string_view status) {
    static const std::regex re("Status: [^\n]*");
    std::string repl = "Status: " + std::string(status);
    if (std::regex_search(body, re)) {
        body = std::regex_replace(body, re, repl, std::regex_constants::format_first_only);
    } else {
        body = repl + "\n" + body;
    }
}

void ensure_exists(std::string_view id, std::string_view project_root) {
    if (fs::exists(entry_path(id, project_root))) return;
    std::ostringstream os;
    os << "# " << id << " (`" << sanitize_id(id) << "`)\n\n"
       << "Status: draft\n"
       << "First seen: " << today_iso() << "\n\n"
       << "## Abstract\n\n(no abstract yet)\n\n"
       << "## Claims\n\n"
       << "## Prior art\n\n"
       << "## Supporting\n";
    write_file(entry_path(id, project_root), os.str());
}

}

void init()     {}
void shutdown() {}

bool exists(std::string_view id, std::string_view project_root) {
    return fs::exists(entry_path(id, project_root));
}

void record_disclosure(std::string_view id,
                       std::string_view title,
                       std::string_view abstract,
                       std::string_view project_root) {
    std::string sid = sanitize_id(id);
    if (sid.empty()) throw std::runtime_error("ip_folder::record_disclosure: empty id");
    auto p = entry_path(sid, project_root);
    std::string body;
    if (fs::exists(p)) {
        body = read_file(p);
    } else {
        std::ostringstream os;
        os << "# " << title << " (`" << sid << "`)\n\n"
           << "Status: draft\n"
           << "First seen: " << today_iso() << "\n\n"
           << "## Abstract\n\n" << abstract << "\n\n"
           << "## Claims\n\n"
           << "## Prior art\n\n"
           << "## Supporting\n";
        body = os.str();
    }
    touch_last(body);
    write_file(p, body);
}

void add_claim(std::string_view id, std::string_view claim, std::string_view root) {
    ensure_exists(id, root);
    auto p = entry_path(id, root);
    std::string body = read_file(p);
    body = append_bullet(body, "## Claims", "- " + today_iso() + ": " + std::string(claim));
    write_file(p, body);
}
void add_prior_art(std::string_view id, std::string_view citation, std::string_view root) {
    ensure_exists(id, root);
    auto p = entry_path(id, root);
    std::string body = read_file(p);
    body = append_bullet(body, "## Prior art", "- " + today_iso() + ": " + std::string(citation));
    write_file(p, body);
}
void add_supporting(std::string_view id, std::string_view ref, std::string_view root) {
    ensure_exists(id, root);
    auto p = entry_path(id, root);
    std::string body = read_file(p);
    body = append_bullet(body, "## Supporting", "- " + today_iso() + ": " + std::string(ref));
    write_file(p, body);
}

void set_status(std::string_view id, std::string_view status, std::string_view root) {
    ensure_exists(id, root);
    auto p = entry_path(id, root);
    std::string body = read_file(p);
    set_status_field(body, status);
    touch_last(body);
    write_file(p, body);
}

std::vector<std::string> list(std::string_view project_root) {
    std::vector<std::string> out;
    fs::path dir = ip_dir(project_root);
    if (!fs::exists(dir)) return out;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        if (e.path().extension() == ".md") out.push_back(e.path().stem().string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

std::optional<Disclosure> read(std::string_view id, std::string_view project_root) {
    auto p = entry_path(id, project_root);
    if (!fs::exists(p)) return std::nullopt;
    Disclosure d;
    d.id    = sanitize_id(id);
    d.body  = read_file(p);
    d.claims     = bullets_in_section(d.body, "## Claims");
    d.prior_art  = bullets_in_section(d.body, "## Prior art");
    d.supporting = bullets_in_section(d.body, "## Supporting");

    auto find_field = [&](const std::string & label) {
        auto pos = d.body.find(label + ": ");
        if (pos == std::string::npos) return std::string{};
        auto eol = d.body.find('\n', pos);
        return d.body.substr(pos + label.size() + 2,
                             eol == std::string::npos ? std::string::npos : eol - (pos + label.size() + 2));
    };
    d.status = find_field("Status");

    auto h1 = d.body.find("# ");
    if (h1 == 0 || (h1 != std::string::npos && (h1 == 0 || d.body[h1 - 1] == '\n'))) {
        auto eol = d.body.find('\n', h1);
        d.title = d.body.substr(h1 + 2, eol == std::string::npos ? std::string::npos : eol - h1 - 2);
    }
    return d;
}

}
