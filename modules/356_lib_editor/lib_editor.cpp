// SPDX-License-Identifier: GPL-3.0-or-later
#include "lib_editor.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <sstream>

namespace lib_editor {

using kicad_model::SchPin;
using kicad_model::Pad;
using kicad_model::LibSymbol;
using kicad_model::Footprint;
using kicad_model::Field;
using kicad_model::make_uuid;

// -------------------- Symbol --------------------

std::string add_pin(SymbolDoc & doc, SchPin pin) {
    if (pin.uuid.empty()) pin.uuid = make_uuid();
    doc.lib.pins.push_back(pin);
    doc.dirty = true;
    return doc.lib.pins.back().uuid;
}
bool remove_pin(SymbolDoc & doc, std::string_view uuid) {
    auto it = std::find_if(doc.lib.pins.begin(), doc.lib.pins.end(),
                           [&](const SchPin & p) { return p.uuid == uuid; });
    if (it == doc.lib.pins.end()) return false;
    doc.lib.pins.erase(it);
    doc.dirty = true;
    return true;
}
bool move_pin(SymbolDoc & doc, std::string_view uuid, long long dx, long long dy) {
    for (auto & p : doc.lib.pins) {
        if (p.uuid == uuid) { p.at.x += dx; p.at.y += dy; doc.dirty = true; return true; }
    }
    return false;
}
void set_field(SymbolDoc & doc, std::string name, std::string value) {
    for (auto & f : doc.lib.fields) {
        if (f.name == name) { f.value = std::move(value); doc.dirty = true; return; }
    }
    Field f; f.name = std::move(name); f.value = std::move(value); f.uuid = make_uuid();
    doc.lib.fields.push_back(f);
    doc.dirty = true;
}

// -------------------- Footprint --------------------

std::string add_pad(FootprintDoc & doc, Pad pad) {
    if (pad.uuid.empty()) pad.uuid = make_uuid();
    doc.fp.pads.push_back(pad);
    doc.dirty = true;
    return doc.fp.pads.back().uuid;
}
bool remove_pad(FootprintDoc & doc, std::string_view uuid) {
    auto it = std::find_if(doc.fp.pads.begin(), doc.fp.pads.end(),
                           [&](const Pad & p) { return p.uuid == uuid; });
    if (it == doc.fp.pads.end()) return false;
    doc.fp.pads.erase(it);
    doc.dirty = true;
    return true;
}
bool move_pad(FootprintDoc & doc, std::string_view uuid, long long dx, long long dy) {
    for (auto & p : doc.fp.pads) {
        if (p.uuid == uuid) { p.at.x += dx; p.at.y += dy; doc.dirty = true; return true; }
    }
    return false;
}

void attach_3d_model(FootprintDoc & doc, std::string_view path,
                     double ox, double oy, double oz,
                     double rx, double ry, double rz,
                     double sx, double sy, double sz) {
    std::ostringstream os;
    os << "(model \"" << path << "\"\n"
       << "\t(offset (xyz " << ox << " " << oy << " " << oz << "))\n"
       << "\t(scale  (xyz " << sx << " " << sy << " " << sz << "))\n"
       << "\t(rotate (xyz " << rx << " " << ry << " " << rz << "))\n"
       << ")";
    doc.fp.raw_graphics_sexpr.push_back(os.str());
    doc.dirty = true;
}

// -------------------- Mesh summariser --------------------

namespace {

MeshSummary summarize_wrl(std::string_view text) {
    MeshSummary out; out.format = "wrl";
    // Count "Coordinate" nodes' point arrays for a vertex estimate; count
    // coordIndex or IndexedFaceSet index entries for face estimate.
    // MVP: cheap textual heuristic.
    std::size_t verts = 0, faces = 0;
    auto count_word = [&](const char * w) {
        std::size_t n = 0, p = 0;
        while ((p = text.find(w, p)) != std::string_view::npos) { ++n; p += std::strlen(w); }
        return n;
    };
    verts = count_word("point [");
    faces = count_word("coordIndex [");
    out.vertices = verts;
    out.faces    = faces;
    // AABB via a coarse pass: look for the first "point [" block and
    // parse triples until "]".
    auto pos = text.find("point [");
    if (pos != std::string_view::npos) {
        auto end = text.find(']', pos);
        auto slice = text.substr(pos + 7, end == std::string_view::npos ? 0 : end - pos - 7);
        double x, y, z; std::size_t idx = 0;
        double lox=1e300, loy=1e300, loz=1e300, hix=-1e300, hiy=-1e300, hiz=-1e300;
        std::string tmp(slice);
        char * p = tmp.data(); char * endptr = nullptr;
        while (*p) {
            x = std::strtod(p, &endptr); if (endptr == p) break; p = endptr;
            y = std::strtod(p, &endptr); if (endptr == p) break; p = endptr;
            z = std::strtod(p, &endptr); if (endptr == p) break; p = endptr;
            lox = std::min(lox, x); hix = std::max(hix, x);
            loy = std::min(loy, y); hiy = std::max(hiy, y);
            loz = std::min(loz, z); hiz = std::max(hiz, z);
            ++idx;
        }
        if (idx > 0) {
            out.lo_x = lox; out.lo_y = loy; out.lo_z = loz;
            out.hi_x = hix; out.hi_y = hiy; out.hi_z = hiz;
        }
    }
    return out;
}

MeshSummary summarize_stl_ascii(std::string_view text) {
    MeshSummary out; out.format = "stl";
    std::size_t facets = 0;
    std::size_t p = 0;
    while ((p = text.find("facet normal", p)) != std::string_view::npos) { ++facets; p += 12; }
    out.faces    = facets;
    out.vertices = facets * 3;
    return out;
}

} // namespace

MeshSummary summarize_mesh(std::string_view text, std::string_view ext) {
    std::string e(ext);
    std::transform(e.begin(), e.end(), e.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (e == "wrl" || e == "vrml" || e == "x3d")
        return summarize_wrl(text);
    if (e == "stl") {
        // Binary STL starts with 80-byte header + uint32 facet count.
        if (text.size() > 84) {
            std::uint32_t n;
            std::memcpy(&n, text.data() + 80, 4);
            MeshSummary out; out.format = "stl";
            out.faces    = n;
            out.vertices = n * 3;
            return out;
        }
        return summarize_stl_ascii(text);
    }
    // STEP: a real parser is out of scope. Report format only.
    if (e == "step" || e == "stp") { MeshSummary out; out.format = "step"; return out; }
    MeshSummary out; out.format = "unknown"; return out;
}

} // namespace lib_editor
