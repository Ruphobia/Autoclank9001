// SPDX-License-Identifier: GPL-3.0-or-later
#include "kicad_libs.hpp"

#include <algorithm>
#include <cctype>
#include <dirent.h>
#include <fstream>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

namespace kicad_libs {
namespace {

std::mutex g_mtx;
Config     g_cfg;
bool       g_ready = false;

// In-memory catalog for the MVP. A follow-on pass moves this to SQLite FTS5.
std::vector<SymbolHit>    g_syms;
std::vector<FootprintHit> g_fps;

bool is_dir(const std::string & p) {
    struct stat st{};
    return ::stat(p.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

bool ends_with(const std::string & s, std::string_view suffix) {
    return s.size() >= suffix.size() &&
           std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

std::string to_lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return out;
}

// Very small .kicad_sym walker: extract "(symbol \"lib:name\" ..." at any
// depth. Deliberately not a real parser; we only need names and libs.
// Works for KiCad 6/7/8/9/10 native format.
void scan_symbol_file(const std::string & path,
                      std::vector<SymbolHit> & out) {
    std::ifstream f(path);
    if (!f) return;
    std::string lib_default;
    {
        // Derive the lib name from the file stem.
        auto slash = path.find_last_of('/');
        auto dot   = path.find_last_of('.');
        if (dot != std::string::npos && (slash == std::string::npos || dot > slash))
            lib_default = path.substr(slash == std::string::npos ? 0 : slash + 1,
                                     dot - (slash == std::string::npos ? 0 : slash + 1));
    }
    std::string line;
    while (std::getline(f, line)) {
        // Match a line like: <ws>(symbol "Name" ...
        auto p = line.find("(symbol \"");
        if (p == std::string::npos) continue;
        p += std::string("(symbol \"").size();
        auto q = line.find('"', p);
        if (q == std::string::npos) continue;
        std::string full_name = line.substr(p, q - p);
        // Skip child symbols: KiCad emits (symbol "PARENT_0_1" ...) inside
        // a parent. Those contain an underscore + digit tail. The top-level
        // library entry has no underscore-digit suffix in the last two
        // segments. Cheap heuristic: skip if it ends with _NN_NN.
        {
            auto is_all_digits = [](std::string_view sv) {
                if (sv.empty()) return false;
                for (char c : sv) if (!std::isdigit(static_cast<unsigned char>(c))) return false;
                return true;
            };
            auto ul = full_name.find_last_of('_');
            if (ul != std::string::npos) {
                std::string tail = full_name.substr(ul + 1);
                auto ul2 = full_name.find_last_of('_', ul - (ul == 0 ? 0 : 1));
                if (is_all_digits(tail) && ul2 != std::string::npos) {
                    std::string mid = full_name.substr(ul2 + 1, ul - ul2 - 1);
                    if (is_all_digits(mid)) continue;
                }
            }
        }
        SymbolHit h;
        auto colon = full_name.find(':');
        if (colon != std::string::npos) {
            h.lib  = full_name.substr(0, colon);
            h.name = full_name.substr(colon + 1);
        } else {
            h.lib  = lib_default;
            h.name = full_name;
        }
        h.source_path = path;
        out.push_back(std::move(h));
    }
}

// Walk a directory tree for .kicad_sym files (symbol libraries).
void scan_symbol_root(const std::string & root, std::vector<SymbolHit> & out,
                      std::size_t & lib_count) {
    if (!is_dir(root)) return;
    DIR * d = ::opendir(root.c_str());
    if (!d) return;
    while (auto * ent = ::readdir(d)) {
        std::string name = ent->d_name;
        if (name == "." || name == "..") continue;
        std::string full = root + "/" + name;
        struct stat st{};
        if (::stat(full.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            scan_symbol_root(full, out, lib_count);
        } else if (ends_with(name, ".kicad_sym")) {
            ++lib_count;
            scan_symbol_file(full, out);
        }
    }
    ::closedir(d);
}

// Walk for *.pretty directories containing *.kicad_mod files.
void scan_footprint_root(const std::string & root, std::vector<FootprintHit> & out,
                         std::size_t & lib_count) {
    if (!is_dir(root)) return;
    DIR * d = ::opendir(root.c_str());
    if (!d) return;
    while (auto * ent = ::readdir(d)) {
        std::string name = ent->d_name;
        if (name == "." || name == "..") continue;
        std::string full = root + "/" + name;
        struct stat st{};
        if (::stat(full.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            if (ends_with(name, ".pretty")) {
                ++lib_count;
                std::string lib = name.substr(0, name.size() - std::string(".pretty").size());
                DIR * dd = ::opendir(full.c_str());
                if (!dd) continue;
                while (auto * f = ::readdir(dd)) {
                    std::string fn = f->d_name;
                    if (!ends_with(fn, ".kicad_mod")) continue;
                    FootprintHit h;
                    h.lib  = lib;
                    h.name = fn.substr(0, fn.size() - std::string(".kicad_mod").size());
                    h.source_path = full + "/" + fn;
                    out.push_back(std::move(h));
                }
                ::closedir(dd);
            } else {
                scan_footprint_root(full, out, lib_count);
            }
        }
    }
    ::closedir(d);
}

// Best-guess roots. Preference order:
//   1. TOOL_KICAD_SYMBOL_ROOT / TOOL_KICAD_FOOTPRINT_ROOT env vars.
//   2. Bundled vendor copy under third_party/kicad-symbols etc. when we
//      add it in the roadmap.
//   3. System install: /usr/share/kicad/symbols and .../footprints.
//   4. The locally built KiCad's demos as a smoke fallback so init doesn't
//      look completely empty even without the shared libs.
void resolve_roots() {
    if (const char * s = std::getenv("TOOL_KICAD_SYMBOL_ROOT"))
        if (is_dir(s)) g_cfg.symbol_root = s;
    if (const char * f = std::getenv("TOOL_KICAD_FOOTPRINT_ROOT"))
        if (is_dir(f)) g_cfg.footprint_root = f;
    if (const char * p = std::getenv("TOOL_KICAD_PACKAGE3D_ROOT"))
        if (is_dir(p)) g_cfg.package3d_root = p;

    if (g_cfg.symbol_root.empty()) {
        for (const char * c : {"kicad/kicad-symbols",
                               "/usr/share/kicad/symbols"}) {
            if (is_dir(c)) { g_cfg.symbol_root = c; break; }
        }
    }
    if (g_cfg.footprint_root.empty()) {
        for (const char * c : {"kicad/kicad-footprints",
                               "/usr/share/kicad/footprints"}) {
            if (is_dir(c)) { g_cfg.footprint_root = c; break; }
        }
    }
    if (g_cfg.package3d_root.empty()) {
        for (const char * c : {"kicad/kicad-packages3D",
                               "/usr/share/kicad/3dmodels"}) {
            if (is_dir(c)) { g_cfg.package3d_root = c; break; }
        }
    }
}

} // namespace

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_ready) return;
    g_cfg   = {};
    g_syms.clear();
    g_fps.clear();
    resolve_roots();
    scan_symbol_root   (g_cfg.symbol_root,    g_syms, g_cfg.symbol_libs);
    scan_footprint_root(g_cfg.footprint_root, g_fps,  g_cfg.footprint_libs);
    g_cfg.ready = !(g_syms.empty() && g_fps.empty());
    g_ready = true;
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_cfg   = {};
    g_syms.clear();
    g_fps.clear();
    g_ready = false;
}

const Config & config() { return g_cfg; }

std::vector<SymbolHit> search_symbols(std::string_view query, std::size_t limit) {
    std::vector<SymbolHit> out;
    std::string q = to_lower(query);
    if (q.empty()) return out;
    for (const auto & h : g_syms) {
        if (to_lower(h.name).find(q) != std::string::npos ||
            (h.lib  == q) ||
            to_lower(h.description).find(q) != std::string::npos) {
            out.push_back(h);
            if (out.size() >= limit) break;
        }
    }
    return out;
}

std::vector<FootprintHit> search_footprints(std::string_view query, std::size_t limit) {
    std::vector<FootprintHit> out;
    std::string q = to_lower(query);
    if (q.empty()) return out;
    for (const auto & h : g_fps) {
        if (to_lower(h.name).find(q) != std::string::npos ||
            to_lower(h.lib).find(q)  != std::string::npos) {
            out.push_back(h);
            if (out.size() >= limit) break;
        }
    }
    return out;
}

namespace {

bool parse_lib_id(std::string_view lib_id, std::string & lib_out, std::string & name_out) {
    auto colon = lib_id.find(':');
    if (colon == std::string_view::npos) return false;
    lib_out.assign(lib_id.substr(0, colon));
    name_out.assign(lib_id.substr(colon + 1));
    return !lib_out.empty() && !name_out.empty();
}

// Read a whole file into a string. Returns empty on failure.
std::string slurp(const std::string & path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    std::string out;
    f.seekg(0, std::ios::end);
    auto sz = f.tellg();
    if (sz > 0) out.reserve(static_cast<std::size_t>(sz));
    f.seekg(0);
    out.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
    return out;
}

// Given text and a starting offset pointing at the '(' of a form, find
// the matching close paren. Naive brace matcher, but s-expressions in
// KiCad libraries never contain "(" or ")" inside strings after a token
// (all string escaping is done with \" and everything inside "..." is
// treated literally). Handles that plus line comments (# to EOL).
std::size_t match_paren(std::string_view s, std::size_t open) {
    if (open >= s.size() || s[open] != '(') return std::string_view::npos;
    int depth = 0;
    bool in_str = false;
    for (std::size_t i = open; i < s.size(); ++i) {
        char c = s[i];
        if (in_str) {
            if (c == '\\' && i + 1 < s.size()) { ++i; continue; }
            if (c == '"') in_str = false;
            continue;
        }
        if (c == '"') { in_str = true; continue; }
        if (c == '(') ++depth;
        else if (c == ')') {
            if (--depth == 0) return i;
        }
    }
    return std::string_view::npos;
}

} // namespace

SymbolHit find_symbol(std::string_view lib_id) {
    std::string lib, name;
    if (!parse_lib_id(lib_id, lib, name)) return {};
    for (const auto & h : g_syms) {
        if (h.lib == lib && h.name == name) return h;
    }
    return {};
}

FootprintHit find_footprint(std::string_view lib_id) {
    std::string lib, name;
    if (!parse_lib_id(lib_id, lib, name)) return {};
    for (const auto & h : g_fps) {
        if (h.lib == lib && h.name == name) return h;
    }
    return {};
}

std::string extract_symbol_block(std::string_view lib_id) {
    SymbolHit hit = find_symbol(lib_id);
    if (hit.source_path.empty()) return {};
    std::string text = slurp(hit.source_path);
    if (text.empty()) return {};

    // Locate the exact (symbol "name" ... ) block. We search for the
    // literal token `(symbol "name"`. Prefer bare `name` first; also
    // accept `lib:name` for files that use the fully-qualified form.
    std::string needle_bare  = std::string("(symbol \"") + hit.name + "\"";
    std::string needle_qual  = std::string("(symbol \"") + hit.lib + ":" + hit.name + "\"";

    std::size_t pos = text.find(needle_qual);
    if (pos == std::string::npos) pos = text.find(needle_bare);
    if (pos == std::string::npos) return {};

    std::size_t end = match_paren(text, pos);
    if (end == std::string::npos) return {};
    return text.substr(pos, end - pos + 1);
}

std::string extract_footprint_block(std::string_view lib_id) {
    FootprintHit hit = find_footprint(lib_id);
    if (hit.source_path.empty()) return {};
    return slurp(hit.source_path);
}

} // namespace kicad_libs
