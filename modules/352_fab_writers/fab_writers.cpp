// SPDX-License-Identifier: GPL-3.0-or-later
#include "fab_writers.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace fab_writers {

namespace {

using kicad_model::Board;
using kicad_model::Footprint;
using kicad_model::Pad;
using kicad_model::PcbTrack;
using kicad_model::PcbArc;
using kicad_model::PcbVia;
using kicad_model::GrLine;
using kicad_model::GrArc;
using kicad_model::GrCircle;
using kicad_model::GrPolygon;
using kicad_model::ItemType;
using geom::VECTOR2I;
using geom::nm_to_mm;
using geom::mm_to_nm;

// Convert nanometers to a gerber unit string using the specified
// integer.decimal precision, with leading-zero suppression.
std::string gerber_num(long long nm, int int_digits, int dec_digits) {
    // Coordinate value = int * 10^(dec) + dec (as one integer).
    // Convert nm to mm as scaled integer with dec_digits decimal places.
    // 1 mm = 1e6 nm. For dec=6, scaled = nm exactly.
    double mm = nm_to_mm(nm);
    long long total = static_cast<long long>(std::llround(mm * std::pow(10.0, dec_digits)));
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%lld", total);
    // Pad or truncate to int_digits + dec_digits total width when using
    // omit-leading form (per Ucamco default in KiCad's writer).
    std::string s = buf;
    // No suppression here; gerber readers accept full-width numbers.
    return s;
}

std::string world_pad_center_str(const Footprint & fp, const Pad & p,
                                 int int_digits, int dec_digits) {
    double a = fp.angle.rad();
    double c = std::cos(a), s = std::sin(a);
    long long lx = p.at.x, ly = p.at.y;
    long long wx = fp.at.x + static_cast<long long>(std::llround(lx * c - ly * s));
    long long wy = fp.at.y + static_cast<long long>(std::llround(lx * s + ly * c));
    return "X" + gerber_num(wx, int_digits, dec_digits) +
           "Y" + gerber_num(-wy, int_digits, dec_digits); // gerber Y is up-positive
}

// Aperture: a circle of a given diameter (nm), or a rectangle (w x h),
// or a roundrect (w x h with a ratio), or an obround (oval).
struct Ap {
    enum Kind { Circle, Rect, Obround, Roundrect } kind;
    long long a = 0;  // diameter for circle; w for rect/obround/roundrect
    long long b = 0;  // ignored for circle; h for others
    double    rr = 0; // roundrect ratio
    bool operator<(const Ap & o) const {
        return std::tie(kind, a, b, rr) < std::tie(o.kind, o.a, o.b, o.rr);
    }
};

std::string ap_body(const Ap & ap) {
    std::ostringstream os;
    switch (ap.kind) {
        case Ap::Circle:    os << "C," << nm_to_mm(ap.a); break;
        case Ap::Rect:      os << "R," << nm_to_mm(ap.a) << "X" << nm_to_mm(ap.b); break;
        case Ap::Obround:   os << "O," << nm_to_mm(ap.a) << "X" << nm_to_mm(ap.b); break;
        case Ap::Roundrect: {
            // Standard roundrect via macro. For MVP emit obround as approximation.
            os << "O," << nm_to_mm(ap.a) << "X" << nm_to_mm(ap.b);
            break;
        }
    }
    return os.str();
}

} // namespace

std::string write_gerber_layer(const Board & board, std::string_view layer_name,
                               const GerberOptions & opts) {
    std::ostringstream os;
    // Header comments.
    os << "G04 Gerber Fmt " << opts.integer_digits << "." << opts.decimal_digits
       << ", GenBy ac9/fab_writers*\n";
    for (const auto & c : opts.header_comments) os << "G04 " << c << "*\n";
    if (opts.include_x2) {
        os << "%TF.GenerationSoftware,ac9,fab_writers,0.1*%\n";
        os << "%TF.CreationDate,generated*%\n";
        os << "%TF.FileFunction,Copper,L1,Top*%\n"; // simplified
    }
    // Format spec: leading zero omission, absolute coords, integer.decimal.
    char fmt[64];
    std::snprintf(fmt, sizeof(fmt), "%%FSLAX%d%dY%d%d*%%\n",
                  opts.integer_digits, opts.decimal_digits,
                  opts.integer_digits, opts.decimal_digits);
    os << fmt;
    os << "%MOMM*%\n";
    os << "G01*\n";  // linear interpolation mode
    os << "G75*\n";  // multi-quadrant arc mode
    os << "%LPD*%\n";// dark polarity

    // Collect apertures.
    std::map<Ap, int> aps;
    int next_ap = 10;
    auto ap_id = [&](Ap a) -> int {
        auto it = aps.find(a);
        if (it != aps.end()) return it->second;
        int id = next_ap++;
        aps[a] = id;
        return id;
    };

    // Enumerate ops we will emit. Two passes: first pass builds aperture
    // table, second pass emits ops.
    struct FlashOp { int ap; std::string xy_str; };
    struct DrawOp  { int ap; std::string from_xy; std::string to_xy; };

    std::vector<FlashOp> flashes;
    std::vector<DrawOp>  draws;

    auto is_copper = (layer_name == "F.Cu" || layer_name == "B.Cu");
    auto is_silk   = (layer_name == "F.SilkS" || layer_name == "B.SilkS");
    auto is_mask   = (layer_name == "F.Mask"  || layer_name == "B.Mask");
    auto is_edge   = (layer_name == "Edge.Cuts");
    (void) is_silk; (void) is_mask; (void) is_edge;

    // Pads on this layer -> flashes.
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint *>(it.get());
        for (const auto & p : fp->pads) {
            bool on_layer = false;
            for (const auto & L : p.layers) {
                if (L == layer_name) { on_layer = true; break; }
                if (L == "*.Cu" && is_copper) { on_layer = true; break; }
            }
            if (!on_layer) continue;

            Ap ap;
            if      (p.shape == "circle")    { ap.kind = Ap::Circle;    ap.a = p.size.x; }
            else if (p.shape == "rect")      { ap.kind = Ap::Rect;      ap.a = p.size.x; ap.b = p.size.y; }
            else if (p.shape == "oval")      { ap.kind = Ap::Obround;   ap.a = p.size.x; ap.b = p.size.y; }
            else                             { ap.kind = Ap::Roundrect; ap.a = p.size.x; ap.b = p.size.y; ap.rr = p.roundrect_ratio; }
            int id = ap_id(ap);
            flashes.push_back({ id, world_pad_center_str(*fp, p, opts.integer_digits, opts.decimal_digits) });
        }
    }

    // Tracks on this layer -> draws.
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbTrack) continue;
        const auto * t = static_cast<const PcbTrack *>(it.get());
        if (t->layer != layer_name && !(is_edge && t->layer == "Edge.Cuts")) continue;
        Ap ap; ap.kind = Ap::Circle; ap.a = t->width_nm;
        int id = ap_id(ap);
        std::string a = "X" + gerber_num(t->start.x, opts.integer_digits, opts.decimal_digits) +
                        "Y" + gerber_num(-t->start.y, opts.integer_digits, opts.decimal_digits);
        std::string b = "X" + gerber_num(t->end.x,   opts.integer_digits, opts.decimal_digits) +
                        "Y" + gerber_num(-t->end.y,   opts.integer_digits, opts.decimal_digits);
        draws.push_back({ id, a, b });
    }

    // Edge cuts (gr_line on Edge.Cuts).
    if (is_edge) {
        for (const auto & it : board.items) {
            if (it->type != ItemType::PcbGrLine) continue;
            const auto * g = static_cast<const GrLine *>(it.get());
            if (g->layer != "Edge.Cuts") continue;
            Ap ap; ap.kind = Ap::Circle; ap.a = g->width_nm > 0 ? g->width_nm : mm_to_nm(0.05);
            int id = ap_id(ap);
            std::string a = "X" + gerber_num(g->start.x, opts.integer_digits, opts.decimal_digits) +
                            "Y" + gerber_num(-g->start.y, opts.integer_digits, opts.decimal_digits);
            std::string b = "X" + gerber_num(g->end.x,   opts.integer_digits, opts.decimal_digits) +
                            "Y" + gerber_num(-g->end.y,   opts.integer_digits, opts.decimal_digits);
            draws.push_back({ id, a, b });
        }
    }

    // Vias on copper layers act as circle flashes.
    if (is_copper) {
        for (const auto & it : board.items) {
            if (it->type != ItemType::PcbVia) continue;
            const auto * v = static_cast<const PcbVia *>(it.get());
            Ap ap; ap.kind = Ap::Circle; ap.a = v->size_nm;
            int id = ap_id(ap);
            std::string s = "X" + gerber_num(v->at.x, opts.integer_digits, opts.decimal_digits) +
                            "Y" + gerber_num(-v->at.y, opts.integer_digits, opts.decimal_digits);
            flashes.push_back({ id, s });
        }
    }

    // Emit aperture defs.
    for (const auto & kv : aps) {
        os << "%ADD" << kv.second << ap_body(kv.first) << "*%\n";
    }

    // Emit ops. Draws first (so pads sit on top).
    int current_ap = -1;
    for (const auto & d : draws) {
        if (d.ap != current_ap) { os << "D" << d.ap << "*\n"; current_ap = d.ap; }
        os << d.from_xy << "D02*\n";
        os << d.to_xy   << "D01*\n";
    }
    for (const auto & f : flashes) {
        if (f.ap != current_ap) { os << "D" << f.ap << "*\n"; current_ap = f.ap; }
        os << f.xy_str << "D03*\n";
    }

    os << "M02*\n";
    return os.str();
}

std::string write_drill_pth(const Board & board, const DrillOptions & opts) {
    std::ostringstream os;
    // Header.
    os << "M48\n"
       << ";FORMAT={-:-/ absolute / metric / suppressed trailing zeros}\n"
       << ";GENBY=ac9/fab_writers 0.1\n"
       << "METRIC,TZ\n"
       << "FMAT,2\n";

    // Collect distinct drill sizes.
    std::map<long long, int> sizes;  // nm -> T-code
    auto tcode = [&](long long nm) {
        auto it = sizes.find(nm);
        if (it != sizes.end()) return it->second;
        int t = static_cast<int>(sizes.size()) + 1;
        sizes[nm] = t;
        return t;
    };

    struct Hole { long long nm; VECTOR2I at; };
    std::vector<Hole> holes;

    for (const auto & it : board.items) {
        if (it->type == ItemType::PcbFootprint) {
            const auto * fp = static_cast<const Footprint *>(it.get());
            for (const auto & p : fp->pads) {
                if (p.drill_nm <= 0) continue;
                if (p.kind != "thru_hole") continue;
                double a = fp->angle.rad();
                double c = std::cos(a), s = std::sin(a);
                long long lx = p.at.x, ly = p.at.y;
                VECTOR2I w{ fp->at.x + static_cast<long long>(std::llround(lx * c - ly * s)),
                            fp->at.y + static_cast<long long>(std::llround(lx * s + ly * c)) };
                (void) tcode(p.drill_nm);
                holes.push_back({ p.drill_nm, w });
            }
        } else if (it->type == ItemType::PcbVia) {
            const auto * v = static_cast<const PcbVia *>(it.get());
            if (v->drill_nm <= 0) continue;
            (void) tcode(v->drill_nm);
            holes.push_back({ v->drill_nm, v->at });
        }
    }

    // Tool defs.
    for (const auto & kv : sizes) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "T%dC%.3f\n", kv.second, nm_to_mm(kv.first));
        os << buf;
    }
    os << "%\n"
       << "G90\nG05\n";
    // Emit holes grouped by tool.
    for (const auto & kv : sizes) {
        os << "T" << kv.second << "\n";
        for (const auto & h : holes) {
            if (h.nm != kv.first) continue;
            char buf[64];
            std::snprintf(buf, sizeof(buf), "X%.4fY%.4f\n",
                          nm_to_mm(h.at.x), nm_to_mm(-h.at.y));
            os << buf;
        }
    }
    os << "T0\nM30\n";
    return os.str();
}

std::string write_drill_npth(const Board & board, const DrillOptions & opts) {
    // Same shape as PTH but filtered on np_thru_hole.
    std::ostringstream os;
    os << "M48\n"
       << ";FORMAT={-:-/ absolute / metric / suppressed trailing zeros}\n"
       << ";GENBY=ac9/fab_writers 0.1 (NPTH)\n"
       << "METRIC,TZ\n"
       << "FMAT,2\n";
    std::map<long long, int> sizes;
    auto tcode = [&](long long nm) {
        auto it = sizes.find(nm);
        if (it != sizes.end()) return it->second;
        int t = static_cast<int>(sizes.size()) + 1;
        sizes[nm] = t;
        return t;
    };
    struct Hole { long long nm; VECTOR2I at; };
    std::vector<Hole> holes;
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint *>(it.get());
        for (const auto & p : fp->pads) {
            if (p.kind != "np_thru_hole" || p.drill_nm <= 0) continue;
            double a = fp->angle.rad();
            double c = std::cos(a), s = std::sin(a);
            long long lx = p.at.x, ly = p.at.y;
            VECTOR2I w{ fp->at.x + static_cast<long long>(std::llround(lx * c - ly * s)),
                        fp->at.y + static_cast<long long>(std::llround(lx * s + ly * c)) };
            (void) tcode(p.drill_nm);
            holes.push_back({ p.drill_nm, w });
        }
    }
    for (const auto & kv : sizes) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "T%dC%.3f\n", kv.second, nm_to_mm(kv.first));
        os << buf;
    }
    os << "%\nG90\nG05\n";
    for (const auto & kv : sizes) {
        os << "T" << kv.second << "\n";
        for (const auto & h : holes) {
            if (h.nm != kv.first) continue;
            char buf[64];
            std::snprintf(buf, sizeof(buf), "X%.4fY%.4f\n",
                          nm_to_mm(h.at.x), nm_to_mm(-h.at.y));
            os << buf;
        }
    }
    os << "T0\nM30\n";
    return os.str();
}

std::string write_job_file(const Board & board,
                           const std::unordered_map<std::string, std::string> & layer_files,
                           const std::string & drill_file) {
    std::ostringstream os;
    os << "{\n"
       << "  \"Header\": {\n"
       << "    \"GenerationSoftware\": { \"Vendor\":\"ac9\", \"Application\":\"fab_writers\", \"Version\":\"0.1\" },\n"
       << "    \"CreationDate\": \"generated\"\n"
       << "  },\n"
       << "  \"GeneralSpecs\": { \"ProjectId\": { \"Name\":\"" << board.uuid << "\" },\n"
       << "    \"Size\": {}, \"LayerNumber\":" << board.layers.size()
       << ", \"BoardThickness\":" << board.thickness_mm << " },\n"
       << "  \"MaterialStackup\": [],\n"
       << "  \"FilesAttributes\": [\n";
    bool first = true;
    for (const auto & kv : layer_files) {
        if (!first) os << ",\n";
        first = false;
        os << "    {\"Path\":\"" << kv.second << "\", \"FileFunction\":\"" << kv.first << "\"}";
    }
    os << "\n  ],\n"
       << "  \"DrillFiles\": [";
    if (!drill_file.empty())
        os << "{\"Path\":\"" << drill_file << "\", \"FileFunction\":\"Plated,1,2,PTH\"}";
    os << "]\n}\n";
    return os.str();
}

std::string write_pos_csv(const Board & board) {
    std::ostringstream os;
    os << "Ref,Value,Package,PosX,PosY,Rot,Side\n";
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint *>(it.get());
        std::string ref, val;
        for (const auto & f : fp->fields) {
            if (f.name == "Reference") ref = f.value;
            if (f.name == "Value")     val = f.value;
        }
        os << ref << "," << val << "," << fp->lib_id << ","
           << nm_to_mm(fp->at.x) << "," << nm_to_mm(fp->at.y) << ","
           << fp->angle.deg() << ","
           << (fp->placement_layer == "B.Cu" ? "bottom" : "top") << "\n";
    }
    return os.str();
}

} // namespace fab_writers
