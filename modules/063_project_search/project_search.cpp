#include "project_search.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <unordered_set>

namespace project_search {
namespace {

namespace fs = std::filesystem;

const std::unordered_set<std::string> & builtin_ignore_dirs() {
    static const std::unordered_set<std::string> s = {
        ".git", ".svn", ".hg", "_deps", "CMakeFiles", "CMakeScripts",
        "build", "node_modules", "venv", ".venv", ".env", "__pycache__",
        "target", ".cache", ".tool"
    };
    return s;
}

bool ignored_dir(const std::string & name,
                 const std::vector<std::string> & extra) {
    if (!name.empty() && name.front() == '.' && name != "." && name != "..") return true;
    if (builtin_ignore_dirs().count(name)) return true;
    for (const auto & e : extra) if (e == name) return true;
    return false;
}

bool extension_ok(const fs::path & p, const std::vector<std::string> & exts) {
    if (exts.empty()) return true;
    std::string e = p.extension().string();
    if (!e.empty() && e.front() == '.') e.erase(0, 1);
    std::transform(e.begin(), e.end(), e.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    for (const auto & w : exts) {
        std::string ww = w;
        std::transform(ww.begin(), ww.end(), ww.begin(),
                       [](unsigned char c){ return (char)std::tolower(c); });
        if (e == ww) return true;
    }
    return false;
}

bool looks_binary(const std::string & body) {
    size_t n = std::min<size_t>(body.size(), 2048);
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)body[i];
        if (c == 0) return true;
    }
    return false;
}

}

void init()     {}
void shutdown() {}

std::vector<Match> search(std::string_view root_v, std::string_view pattern,
                          const Options & opts) {
    std::vector<Match> out;
    fs::path root{std::string(root_v)};
    if (!fs::exists(root)) return out;

    std::regex re;
    try {
        re = std::regex(std::string{pattern},
            opts.case_insensitive
                ? (std::regex::ECMAScript | std::regex::icase)
                : std::regex::ECMAScript);
    } catch (...) { return out; }

    for (auto it = fs::recursive_directory_iterator(root,
             fs::directory_options::skip_permission_denied);
         it != fs::recursive_directory_iterator(); ++it) {
        if (out.size() >= opts.max_matches) break;
        const auto & p = it->path();
        std::string name = p.filename().string();
        if (it->is_directory()) {
            if (ignored_dir(name, opts.ignore_dirs)) it.disable_recursion_pending();
            continue;
        }
        if (!it->is_regular_file()) continue;
        if (!extension_ok(p, opts.include_extensions)) continue;
        if ((size_t)fs::file_size(p) > opts.max_file_bytes) continue;

        std::ifstream in(p, std::ios::binary);
        std::ostringstream b; b << in.rdbuf();
        std::string body = b.str();
        if (looks_binary(body)) continue;

        // Walk lines; cheap manual scan beats std::getline overhead at scale.
        size_t line_start = 0;
        size_t line_no    = 1;
        for (size_t i = 0; i <= body.size(); ++i) {
            if (i == body.size() || body[i] == '\n') {
                std::string_view line(body.data() + line_start, i - line_start);
                std::cmatch m;
                if (std::regex_search(line.data(), line.data() + line.size(), m, re)) {
                    Match mm;
                    mm.path         = p.string();
                    mm.line_number  = line_no;
                    mm.line_text    = std::string(line);
                    mm.match_start  = (size_t)m.position(0);
                    mm.match_length = (size_t)m.length(0);
                    out.push_back(std::move(mm));
                    if (out.size() >= opts.max_matches) break;
                }
                line_start = i + 1;
                ++line_no;
            }
        }
    }
    std::sort(out.begin(), out.end(), [](const Match & a, const Match & b){
        if (a.path != b.path) return a.path < b.path;
        return a.line_number < b.line_number;
    });
    return out;
}

std::vector<Match> search_literal(std::string_view root, std::string_view phrase,
                                  const Options & opts) {
    std::string esc;
    esc.reserve(phrase.size() * 2);
    for (char c : phrase) {
        switch (c) {
            case '\\': case '.': case '+': case '*': case '?':
            case '(': case ')': case '[': case ']': case '{': case '}':
            case '|': case '^': case '$':
                esc.push_back('\\');
                [[fallthrough]];
            default:
                esc.push_back(c);
        }
    }
    return search(root, esc, opts);
}

}
