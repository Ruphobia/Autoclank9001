#include "style_profile.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace style_profile {
namespace {

namespace fs = std::filesystem;

fs::path pick_dir(std::string_view override_root) {
    if (!override_root.empty()) return fs::path(std::string(override_root));
    if (const char * e = std::getenv("TOOL_PERSONAL_DIR")) return fs::path(e);
    if (const char * h = std::getenv("HOME"))              return fs::path(h) / ".tool" / "personal";
    return fs::current_path() / ".tool" / "personal";
}

fs::path file_for(std::string_view override_root) {
    return pick_dir(override_root) / "style.md";
}

std::string trim(std::string_view in) {
    size_t a = 0, b = in.size();
    while (a < b && std::isspace(static_cast<unsigned char>(in[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(in[b - 1]))) --b;
    return std::string(in.substr(a, b - a));
}

std::string read_file(const fs::path & p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) return {};
    std::ostringstream b; b << in.rdbuf();
    return b.str();
}

std::vector<std::string> parse_bullets(const std::string & section) {
    std::vector<std::string> out;
    std::istringstream is(section);
    std::string line;
    while (std::getline(is, line)) {
        std::string t = trim(line);
        if (t.size() >= 2 && t[0] == '-' && std::isspace(static_cast<unsigned char>(t[1]))) {
            out.push_back(trim(t.substr(2)));
        }
    }
    return out;
}

std::string read_section(const std::string & body, const std::string & header) {
    auto pos = body.find(header + "\n");
    if (pos == std::string::npos) return {};
    auto end = body.find("\n## ", pos + 1);
    std::string s = body.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
    auto nl = s.find('\n');
    if (nl == std::string::npos) return {};
    return trim(s.substr(nl + 1));
}

std::string read_field(const std::string & body, const std::string & label) {
    auto pos = body.find(label + ": ");
    if (pos == std::string::npos) return {};
    auto eol = body.find('\n', pos);
    return trim(body.substr(pos + label.size() + 2,
                            eol == std::string::npos ? std::string::npos : eol - (pos + label.size() + 2)));
}

void write_bullets(std::ostringstream & os, const std::vector<std::string> & v) {
    for (const auto & s : v) os << "- " << s << "\n";
    os << "\n";
}

std::string join_csv(const std::vector<std::string> & v) {
    std::string out;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) out += ", ";
        out += v[i];
    }
    return out;
}

}

void init()     {}
void shutdown() {}

std::string profile_path(std::string_view override_root) {
    return file_for(override_root).string();
}

Profile load(std::string_view override_root) {
    Profile p;
    auto path = file_for(override_root);
    if (!fs::exists(path)) return p;
    std::string body = read_file(path);
    p.tone     = read_field(body,   "Tone");
    p.pronouns = read_field(body,   "Pronouns");
    p.units_preferred = parse_bullets(read_section(body, "## Units preferred"));
    p.avoid           = parse_bullets(read_section(body, "## Avoid"));
    p.use             = parse_bullets(read_section(body, "## Use"));
    p.notes           = read_section(body, "## Notes");
    return p;
}

void save(const Profile & p, std::string_view override_root) {
    auto path = file_for(override_root);
    fs::create_directories(path.parent_path());
    std::ostringstream os;
    os << "# Personal style profile\n\n"
       << "Tone: " << p.tone << "\n"
       << "Pronouns: " << p.pronouns << "\n\n"
       << "## Units preferred\n\n";
    write_bullets(os, p.units_preferred);
    os << "## Avoid\n\n"; write_bullets(os, p.avoid);
    os << "## Use\n\n";   write_bullets(os, p.use);
    os << "## Notes\n\n" << p.notes << "\n";
    std::ofstream o(path, std::ios::binary | std::ios::trunc);
    if (!o) throw std::runtime_error("style_profile::save: cannot write " + path.string());
    o << os.str();
}

std::string format_system_prompt(const Profile & p) {
    std::ostringstream os;
    bool any = false;
    if (!p.tone.empty())       { os << "Write in a " << p.tone << " tone. ";  any = true; }
    if (!p.pronouns.empty())   { os << "User pronouns: " << p.pronouns << ". "; any = true; }
    if (!p.units_preferred.empty()) {
        os << "Prefer " << join_csv(p.units_preferred) << " units. "; any = true;
    }
    if (!p.avoid.empty()) {
        os << "Avoid: " << join_csv(p.avoid) << ". "; any = true;
    }
    if (!p.use.empty()) {
        os << "Use: " << join_csv(p.use) << ". "; any = true;
    }
    if (!p.notes.empty()) {
        os << "Notes from user: " << p.notes;
        if (p.notes.back() != '.') os << ".";
        os << " ";
        any = true;
    }
    if (!any) return {};
    std::string s = os.str();
    while (!s.empty() && s.back() == ' ') s.pop_back();
    return s;
}

}
