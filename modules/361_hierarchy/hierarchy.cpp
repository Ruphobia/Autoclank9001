// SPDX-License-Identifier: GPL-3.0-or-later
#include "hierarchy.hpp"

#include "../347_kicad_io/kicad_io.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace hierarchy {

using kicad_model::Schematic;
using kicad_model::SchScreen;
using kicad_model::SchSheet;
using kicad_model::ItemType;

namespace {

std::string resolve_path(std::string_view base, std::string_view rel) {
    if (rel.empty()) return {};
    if (!rel.empty() && rel.front() == '/') return std::string(rel);
    if (base.empty()) return std::string(rel);
    std::string out(base);
    if (out.back() != '/') out += '/';
    out += rel;
    return out;
}

bool file_exists(const std::string & p) {
    struct stat st{};
    return ::stat(p.c_str(), &st) == 0;
}

void load_recursive(Schematic & sch, SchScreen & scr, const LoadOptions & opts,
                    int depth, LoadReport & rep) {
    if (depth > opts.max_depth) {
        rep.warnings.push_back("max depth exceeded at recursion level " + std::to_string(depth));
        return;
    }
    for (const auto & it : scr.items) {
        if (it->type != ItemType::SchSheet) continue;
        const auto * sh = static_cast<const SchSheet*>(it.get());
        std::string file_name;
        for (const auto & f : sh->fields) if (f.name == "Sheetfile") { file_name = f.value; break; }
        if (file_name.empty() && !sh->file_name.empty()) file_name = sh->file_name;
        if (file_name.empty()) continue;

        std::string full = resolve_path(opts.base_dir, file_name);
        if (!file_exists(full)) {
            rep.warnings.push_back("cannot find sheet file: " + full);
            continue;
        }
        kicad_io::IOError err;
        auto child = kicad_io::read_schematic_file(full, &err);
        if (!child) {
            rep.warnings.push_back("read failed: " + full + " (" + err.message + ")");
            continue;
        }
        // Merge the child's lib_symbols into the root.
        for (const auto & kv : child->lib_symbols)
            sch.lib_symbols.emplace(kv.first, kv.second);
        auto & child_scr = sch.child_screens[sh->uuid];
        child_scr = std::move(child->root);
        ++rep.sheets_loaded;
        // Recurse.
        load_recursive(sch, child_scr, opts, depth + 1, rep);
    }
}

} // namespace

LoadReport load_children(Schematic & sch, const LoadOptions & opts) {
    LoadReport rep;
    load_recursive(sch, sch.root, opts, 0, rep);
    return rep;
}

std::vector<kicad_model::ItemPtr> flatten(const Schematic & sch) {
    std::vector<kicad_model::ItemPtr> out;
    out.insert(out.end(), sch.root.items.begin(), sch.root.items.end());
    for (const auto & kv : sch.child_screens) {
        out.insert(out.end(), kv.second.items.begin(), kv.second.items.end());
    }
    return out;
}

std::string Path::display() const {
    std::string s = "/";
    for (const auto & u : stack) { s += u.substr(0, 8); s += "/"; }
    return s;
}

const kicad_model::SchScreen * screen_at(const Schematic & sch, const Path & path) {
    if (path.at_root()) return &sch.root;
    for (const auto & u : path.stack) {
        auto it = sch.child_screens.find(u);
        if (it == sch.child_screens.end()) return nullptr;
        (void) it;
    }
    // Return the last one in the chain.
    auto it = sch.child_screens.find(path.stack.back());
    return it != sch.child_screens.end() ? &it->second : nullptr;
}

} // namespace hierarchy
