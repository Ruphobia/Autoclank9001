// SPDX-License-Identifier: GPL-3.0-or-later
#include "altium_import.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace altium_import {

using kicad_model::Board;

namespace {

// Extract KEY=VALUE tokens from an ASCII line. Values may be quoted.
std::unordered_map<std::string, std::string> parse_kv(std::string_view line) {
    std::unordered_map<std::string, std::string> out;
    std::size_t i = 0;
    while (i < line.size()) {
        while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (i >= line.size()) break;
        auto ks = i;
        while (i < line.size() && line[i] != '=' && !std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        std::string key(line.substr(ks, i - ks));
        if (i < line.size() && line[i] == '=') {
            ++i;
            std::string val;
            if (i < line.size() && line[i] == '"') {
                ++i;
                auto vs = i;
                while (i < line.size() && line[i] != '"') ++i;
                val = std::string(line.substr(vs, i - vs));
                if (i < line.size()) ++i;
            } else {
                auto vs = i;
                while (i < line.size() && !std::isspace(static_cast<unsigned char>(line[i]))) ++i;
                val = std::string(line.substr(vs, i - vs));
            }
            out[key] = std::move(val);
        }
    }
    return out;
}

// Altium ASCII uses mils by default; convert to mm/nm.
double mils_to_mm(double m) { return m * 0.0254; }

std::string map_layer_int(int layer) {
    switch (layer) {
        case 1:  return "F.Cu";
        case 2:  return "B.Cu";
        case 3:  return "F.SilkS";
        case 4:  return "B.SilkS";
        case 5:  return "Edge.Cuts";
        case 6:  return "F.Mask";
        case 7:  return "B.Mask";
        case 8:  return "F.Paste";
        case 9:  return "B.Paste";
        default: return "Dwgs.User";
    }
}

} // namespace

std::optional<Board> read_board(std::string_view text, ImportReport * rep) {
    Board b;
    b.uuid = kicad_model::make_uuid();
    b.layers = kicad_model::default_2layer_stackup();
    kicad_model::intern_net(b, "");

    ImportReport local;
    ImportReport & r = rep ? *rep : local;

    // Nets get interned on first appearance.
    std::unordered_map<std::string, int> net_by_name;
    auto get_net = [&](const std::string & name) -> int {
        if (name.empty() || name == "NoName") return 0;
        auto it = net_by_name.find(name);
        if (it != net_by_name.end()) return it->second;
        int id = static_cast<int>(b.nets.size());
        b.nets.push_back({ id, name });
        net_by_name[name] = id;
        return id;
    };

    std::size_t start = 0;
    bool saw_header = false;
    while (start < text.size()) {
        auto nl = text.find('\n', start);
        std::string_view line_raw = text.substr(start, nl == std::string_view::npos ? text.size() - start : nl - start);
        start = (nl == std::string_view::npos) ? text.size() : nl + 1;
        std::string line(line_raw);
        while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        if (!saw_header) {
            if (line.find("PCB FILE VERSION") != std::string::npos) saw_header = true;
            continue;
        }

        // Records start with the token type in brackets or first word.
        std::size_t p = 0;
        std::string type;
        while (p < line.size() && !std::isspace(static_cast<unsigned char>(line[p]))) ++p;
        type = line.substr(0, p);
        std::transform(type.begin(), type.end(), type.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        std::string_view rest = std::string_view(line).substr(p);
        auto kv = parse_kv(rest);

        auto dv = [&](const char * k, double dflt = 0) {
            auto it = kv.find(k);
            return it == kv.end() ? dflt : std::atof(it->second.c_str());
        };
        auto sv = [&](const char * k, const std::string & dflt = {}) {
            auto it = kv.find(k);
            return it == kv.end() ? dflt : it->second;
        };
        auto iv = [&](const char * k, int dflt = 0) {
            auto it = kv.find(k);
            return it == kv.end() ? dflt : std::atoi(it->second.c_str());
        };

        if (type == "TRACK") {
            auto t = std::make_shared<kicad_model::PcbTrack>();
            t->start.x = geom::mm_to_nm(mils_to_mm(dv("X1")));
            t->start.y = geom::mm_to_nm(mils_to_mm(dv("Y1")));
            t->end.x   = geom::mm_to_nm(mils_to_mm(dv("X2")));
            t->end.y   = geom::mm_to_nm(mils_to_mm(dv("Y2")));
            t->width_nm= geom::mm_to_nm(mils_to_mm(dv("W", 8.0)));
            t->layer   = map_layer_int(iv("L", 1));
            t->net     = get_net(sv("NET"));
            t->uuid    = kicad_model::make_uuid();
            b.items.push_back(t);
            ++r.tracks;
        } else if (type == "VIA") {
            auto v = std::make_shared<kicad_model::PcbVia>();
            v->at.x = geom::mm_to_nm(mils_to_mm(dv("X")));
            v->at.y = geom::mm_to_nm(mils_to_mm(dv("Y")));
            v->size_nm  = geom::mm_to_nm(mils_to_mm(dv("D", 24)));
            v->drill_nm = geom::mm_to_nm(mils_to_mm(dv("H", 12)));
            v->net      = get_net(sv("NET"));
            v->uuid     = kicad_model::make_uuid();
            b.items.push_back(v);
            ++r.vias;
        } else if (type == "COMPONENT") {
            auto fp = std::make_shared<kicad_model::Footprint>();
            std::string ref = sv("REF", sv("REFDES"));
            std::string val = sv("VALUE");
            std::string pkg = sv("PACKAGE", sv("PATTERN"));
            fp->lib_id = "altium:" + (pkg.empty() ? std::string("unknown") : pkg);
            fp->at.x = geom::mm_to_nm(mils_to_mm(dv("X")));
            fp->at.y = geom::mm_to_nm(mils_to_mm(dv("Y")));
            fp->angle = geom::EDA_ANGLE{ dv("ROT", 0.0) };
            fp->placement_layer = iv("L", 1) == 1 ? "F.Cu" : "B.Cu";
            fp->fields.push_back({"Reference", ref, {}, {}, false, false, false, 1, 1, {}, kicad_model::make_uuid()});
            fp->fields.push_back({"Value",     val, {}, {}, false, false, false, 1, 1, {}, kicad_model::make_uuid()});
            fp->uuid = kicad_model::make_uuid();
            b.items.push_back(fp);
            ++r.components;
        } else if (type == "FILL") {
            auto g = std::make_shared<kicad_model::GrPolygon>();
            g->layer = map_layer_int(iv("L", 1));
            g->fill_type = "solid";
            g->outline.append({ geom::mm_to_nm(mils_to_mm(dv("X1"))), geom::mm_to_nm(mils_to_mm(dv("Y1"))) });
            g->outline.append({ geom::mm_to_nm(mils_to_mm(dv("X2"))), geom::mm_to_nm(mils_to_mm(dv("Y1"))) });
            g->outline.append({ geom::mm_to_nm(mils_to_mm(dv("X2"))), geom::mm_to_nm(mils_to_mm(dv("Y2"))) });
            g->outline.append({ geom::mm_to_nm(mils_to_mm(dv("X1"))), geom::mm_to_nm(mils_to_mm(dv("Y2"))) });
            g->outline.set_closed(true);
            g->uuid = kicad_model::make_uuid();
            b.items.push_back(g);
            ++r.fills;
        } else if (type == "PAD") {
            // Treated as a NPTH mounting hole in Edge.Cuts context.
            // Ignored in MVP; belongs to a COMPONENT.
        }
    }

    r.ok = saw_header;
    return b;
}

} // namespace altium_import
