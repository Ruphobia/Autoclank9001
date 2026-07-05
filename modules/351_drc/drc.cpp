// SPDX-License-Identifier: GPL-3.0-or-later
#include "drc.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace drc {

namespace {

using kicad_model::Board;
using kicad_model::Footprint;
using kicad_model::Pad;
using kicad_model::PcbTrack;
using kicad_model::PcbVia;
using kicad_model::GrLine;
using kicad_model::ItemType;
using geom::VECTOR2I;
using geom::VECTOR2D;
using geom::mm_to_nm;
using geom::nm_to_mm;

std::string sev_str(Severity s) {
    switch (s) {
        case Severity::Error:   return "error";
        case Severity::Warning: return "warning";
        case Severity::Info:    return "info";
        case Severity::Ignore:  return "ignore";
    }
    return "info";
}

void tally(Report & r, Violation v) {
    switch (v.severity) {
        case Severity::Error:   ++r.errors;   break;
        case Severity::Warning: ++r.warnings; break;
        case Severity::Info:    ++r.infos;    break;
        case Severity::Ignore:  ++r.ignored;  break;
    }
    r.violations.push_back(std::move(v));
}

// Physical footprint layer where a pad lives (F.Cu / B.Cu / both).
std::vector<std::string> pad_copper_layers(const Pad & p) {
    std::vector<std::string> out;
    for (const auto & L : p.layers) {
        if (L == "F.Cu" || L == "B.Cu" || L == "*.Cu") out.push_back(L);
    }
    // "*.Cu" expands to both.
    if (std::find(out.begin(), out.end(), std::string("*.Cu")) != out.end()) {
        return { "F.Cu", "B.Cu" };
    }
    return out;
}

// World-space center of a pad (footprint transform applied).
VECTOR2I world_pad_center(const Footprint & fp, const Pad & p) {
    // Ignore mirror for now; not commonly used.
    double a = fp.angle.rad();
    double c = std::cos(a), s = std::sin(a);
    long long lx = p.at.x, ly = p.at.y;
    long long wx = fp.at.x + static_cast<long long>(std::llround(lx * c - ly * s));
    long long wy = fp.at.y + static_cast<long long>(std::llround(lx * s + ly * c));
    return { wx, wy };
}

// Effective radius in nm for pad clearance approximation. Uses half of
// the larger dimension of the bounding box (overestimates for rects,
// exact for circles and roundrects). Overestimation is safe for
// clearance: it produces false positives, not false negatives.
long long pad_effective_radius_nm(const Pad & p) {
    return std::max(p.size.x, p.size.y) / 2;
}

// Distance in mm between two pad centers minus their radii.
double pad_pad_clearance_mm(const Footprint & fpa, const Pad & pa,
                            const Footprint & fpb, const Pad & pb) {
    VECTOR2I ca = world_pad_center(fpa, pa);
    VECTOR2I cb = world_pad_center(fpb, pb);
    double d = std::hypot(static_cast<double>(ca.x - cb.x),
                          static_cast<double>(ca.y - cb.y));
    return nm_to_mm(static_cast<long long>(d) - pad_effective_radius_nm(pa) - pad_effective_radius_nm(pb));
}

// Distance from track (segment) to a pad center minus track half-width
// and pad radius.
double track_pad_clearance_mm(const PcbTrack & t,
                              const Footprint & fp, const Pad & p) {
    geom::SEG seg{ t.start, t.end };
    VECTOR2I c = world_pad_center(fp, p);
    double dist = seg.distance(c);
    return nm_to_mm(static_cast<long long>(dist) - t.width_nm / 2 - pad_effective_radius_nm(p));
}

// Distance between two tracks (min of the four segment-to-endpoint
// distances) minus half widths.
double track_track_clearance_mm(const PcbTrack & a, const PcbTrack & b) {
    geom::SEG sa{ a.start, a.end };
    geom::SEG sb{ b.start, b.end };
    // If they intersect anywhere: clearance = 0 (or negative).
    if (sa.intersect(sb)) return -1.0;
    double d1 = sa.distance(b.start);
    double d2 = sa.distance(b.end);
    double d3 = sb.distance(a.start);
    double d4 = sb.distance(a.end);
    double d  = std::min({d1, d2, d3, d4});
    return nm_to_mm(static_cast<long long>(d) - a.width_nm / 2 - b.width_nm / 2);
}

// Hole-to-hole clearance between two drilled items.
double hole_hole_clearance_mm(VECTOR2I ac, long long dr_a,
                              VECTOR2I bc, long long dr_b) {
    double d = std::hypot(static_cast<double>(ac.x - bc.x),
                          static_cast<double>(ac.y - bc.y));
    return nm_to_mm(static_cast<long long>(d) - dr_a / 2 - dr_b / 2);
}

// Return true if two pads may connect (share a net > 0).
bool same_net(const Pad & a, const Pad & b) {
    return a.net > 0 && a.net == b.net;
}
bool same_net(const PcbTrack & a, const PcbTrack & b) {
    return a.net > 0 && a.net == b.net;
}
bool same_net(const PcbTrack & t, const Pad & p) {
    return t.net > 0 && t.net == p.net;
}

// Track+pad share a copper layer?
bool overlap_layer(const PcbTrack & t, const Pad & p) {
    for (const auto & L : pad_copper_layers(p)) if (L == t.layer) return true;
    return false;
}

// Extract every Edge.Cuts segment from the board for edge-clearance checks.
std::vector<geom::SEG> edge_cut_segments(const Board & b) {
    std::vector<geom::SEG> out;
    for (const auto & it : b.items) {
        if (it->type != ItemType::PcbGrLine) continue;
        const auto * g = static_cast<const GrLine *>(it.get());
        if (g->layer == "Edge.Cuts") out.push_back({g->start, g->end});
    }
    return out;
}

} // namespace

Report run(const Board & board, const Config & cfg) {
    Report r;

    // Collect footprints, tracks, vias.
    std::vector<const Footprint *> fps;
    std::vector<const PcbTrack  *> tracks;
    std::vector<const PcbVia    *> vias;
    for (const auto & it : board.items) {
        switch (it->type) {
            case ItemType::PcbFootprint: fps   .push_back(static_cast<const Footprint*>(it.get())); break;
            case ItemType::PcbTrack:     tracks.push_back(static_cast<const PcbTrack  *>(it.get())); break;
            case ItemType::PcbVia:       vias  .push_back(static_cast<const PcbVia    *>(it.get())); break;
            default: break;
        }
    }

    auto ref_of = [](const Footprint * fp) -> std::string {
        for (const auto & f : fp->fields) if (f.name == "Reference") return f.value;
        return fp->lib_id;
    };

    // -------- 1. Pad <-> Pad clearance --------
    for (std::size_t i = 0; i < fps.size(); ++i) {
        for (std::size_t j = i + 1; j < fps.size(); ++j) {
            for (const auto & pa : fps[i]->pads) {
                for (const auto & pb : fps[j]->pads) {
                    if (same_net(pa, pb)) continue;
                    bool overlap = false;
                    auto la = pad_copper_layers(pa);
                    auto lb = pad_copper_layers(pb);
                    for (const auto & xa : la) for (const auto & xb : lb) if (xa == xb) overlap = true;
                    if (!overlap) continue;
                    double c = pad_pad_clearance_mm(*fps[i], pa, *fps[j], pb);
                    if (c < cfg.base_clearance_mm) {
                        Violation v;
                        v.rule_id  = "clearance";
                        v.severity = cfg.clearance;
                        std::ostringstream os;
                        os << "pad " << ref_of(fps[i]) << "." << pa.number
                           << " vs pad " << ref_of(fps[j]) << "." << pb.number
                           << " clearance " << c << " mm < " << cfg.base_clearance_mm << " mm";
                        v.message = os.str();
                        v.has_pos = true;
                        v.at      = world_pad_center(*fps[i], pa);
                        tally(r, std::move(v));
                    }
                }
            }
        }
    }

    // -------- 2. Track <-> Pad clearance --------
    for (const auto * t : tracks) {
        for (const auto * fp : fps) {
            for (const auto & p : fp->pads) {
                if (same_net(*t, p)) continue;
                if (!overlap_layer(*t, p)) continue;
                double c = track_pad_clearance_mm(*t, *fp, p);
                if (c < cfg.base_clearance_mm) {
                    Violation v;
                    v.rule_id  = "clearance";
                    v.severity = cfg.clearance;
                    std::string fp_ref;
                    for (const auto & f : fp->fields) if (f.name == "Reference") { fp_ref = f.value; break; }
                    std::ostringstream os;
                    os << "track (layer " << t->layer << ", net " << t->net << ") vs pad "
                       << fp_ref << "." << p.number
                       << " clearance " << c << " mm < " << cfg.base_clearance_mm << " mm";
                    v.message = os.str();
                    v.has_pos = true;
                    v.at.x    = (t->start.x + t->end.x) / 2;
                    v.at.y    = (t->start.y + t->end.y) / 2;
                    v.layer   = t->layer;
                    tally(r, std::move(v));
                }
            }
        }
    }

    // -------- 3. Track <-> Track clearance --------
    for (std::size_t i = 0; i < tracks.size(); ++i) {
        for (std::size_t j = i + 1; j < tracks.size(); ++j) {
            const auto * a = tracks[i];
            const auto * b = tracks[j];
            if (a->layer != b->layer) continue;
            if (same_net(*a, *b)) continue;
            double c = track_track_clearance_mm(*a, *b);
            if (c < cfg.base_clearance_mm) {
                Violation v;
                v.rule_id  = "clearance";
                v.severity = cfg.clearance;
                std::ostringstream os;
                os << "track (net " << a->net << ") vs track (net " << b->net
                   << ") on " << a->layer << " clearance " << c << " mm";
                v.message = os.str();
                v.has_pos = true;
                v.at.x = (a->start.x + a->end.x) / 2;
                v.at.y = (a->start.y + a->end.y) / 2;
                v.layer = a->layer;
                tally(r, std::move(v));
            }
        }
    }

    // -------- 4. Hole to hole --------
    // Pad holes.
    struct HoleRef { VECTOR2I at; long long drill; std::string label; };
    std::vector<HoleRef> holes;
    for (const auto * fp : fps) {
        for (const auto & p : fp->pads) {
            if (p.drill_nm <= 0) continue;
            holes.push_back({ world_pad_center(*fp, p), p.drill_nm, "pad " + p.number });
        }
    }
    for (const auto * v : vias) {
        if (v->drill_nm <= 0) continue;
        holes.push_back({ v->at, v->drill_nm, "via" });
    }
    for (std::size_t i = 0; i < holes.size(); ++i) {
        for (std::size_t j = i + 1; j < holes.size(); ++j) {
            double c = hole_hole_clearance_mm(holes[i].at, holes[i].drill,
                                              holes[j].at, holes[j].drill);
            if (c < cfg.base_hole_hole_mm) {
                Violation v;
                v.rule_id  = "hole_to_hole";
                v.severity = cfg.hole_to_hole;
                std::ostringstream os;
                os << holes[i].label << " vs " << holes[j].label
                   << " hole clearance " << c << " mm < " << cfg.base_hole_hole_mm << " mm";
                v.message = os.str();
                v.has_pos = true;
                v.at      = holes[i].at;
                tally(r, std::move(v));
            }
        }
    }

    // -------- 5. Copper edge clearance --------
    auto edges = edge_cut_segments(board);
    if (!edges.empty()) {
        for (const auto * fp : fps) {
            for (const auto & p : fp->pads) {
                VECTOR2I c = world_pad_center(*fp, p);
                for (const auto & e : edges) {
                    double d = e.distance(c);
                    double margin = nm_to_mm(static_cast<long long>(d) - pad_effective_radius_nm(p));
                    if (margin < cfg.copper_edge_mm) {
                        Violation v;
                        v.rule_id  = "copper_edge_clearance";
                        v.severity = cfg.copper_edge_clearance;
                        std::ostringstream os;
                        os << "pad " << p.number << " margin " << margin
                           << " mm to Edge.Cuts < " << cfg.copper_edge_mm << " mm";
                        v.message = os.str();
                        v.has_pos = true;
                        v.at      = c;
                        tally(r, std::move(v));
                        break;
                    }
                }
            }
        }
        for (const auto * t : tracks) {
            for (const auto & e : edges) {
                geom::SEG ts{ t->start, t->end };
                double d = std::min(e.distance(t->start), e.distance(t->end));
                double margin = nm_to_mm(static_cast<long long>(d) - t->width_nm / 2);
                if (margin < cfg.copper_edge_mm) {
                    Violation v;
                    v.rule_id  = "copper_edge_clearance";
                    v.severity = cfg.copper_edge_clearance;
                    std::ostringstream os;
                    os << "track margin " << margin << " mm to Edge.Cuts < " << cfg.copper_edge_mm << " mm";
                    v.message = os.str();
                    v.has_pos = true;
                    v.at.x = (t->start.x + t->end.x) / 2;
                    v.at.y = (t->start.y + t->end.y) / 2;
                    tally(r, std::move(v));
                    break;
                }
            }
        }
    }

    // -------- 6. Missing courtyard --------
    for (const auto * fp : fps) {
        bool has_crtyd = false;
        for (const auto & g : fp->raw_graphics_sexpr) {
            if (g.find("F.CrtYd") != std::string::npos ||
                g.find("B.CrtYd") != std::string::npos) { has_crtyd = true; break; }
        }
        if (!has_crtyd) {
            Violation v;
            v.rule_id  = "missing_courtyard";
            v.severity = cfg.missing_courtyard;
            v.message  = "footprint " + fp->lib_id + " has no courtyard graphics";
            v.has_pos  = true;
            v.at       = fp->at;
            tally(r, std::move(v));
        }
    }

    // -------- 7. Unconnected pads on net 0 --------
    for (const auto * fp : fps) {
        for (const auto & p : fp->pads) {
            if (p.net == 0 && (p.kind == "smd" || p.kind == "thru_hole")) {
                Violation v;
                v.rule_id  = "unconnected_items";
                v.severity = cfg.unconnected_items;
                v.message  = "pad " + fp->lib_id + "." + p.number + " is not assigned to a net";
                v.has_pos  = true;
                v.at       = world_pad_center(*fp, p);
                tally(r, std::move(v));
            }
        }
    }

    // -------- 8. Annular ring width --------
    for (const auto * fp : fps) {
        for (const auto & p : fp->pads) {
            if (p.drill_nm <= 0) continue;
            long long pad_size_min = std::min(p.size.x, p.size.y);
            double ann_mm = nm_to_mm((pad_size_min - p.drill_nm) / 2);
            if (ann_mm < cfg.min_annular_mm) {
                Violation v;
                v.rule_id  = "annular_width";
                v.severity = cfg.annular_width;
                std::ostringstream os;
                os << "pad " << p.number << " annular " << ann_mm << " mm < " << cfg.min_annular_mm << " mm";
                v.message = os.str();
                v.has_pos = true;
                v.at      = world_pad_center(*fp, p);
                tally(r, std::move(v));
            }
        }
    }

    // -------- 9. Silk over copper (footprint pad on same side) --------
    for (const auto * fp : fps) {
        bool has_silk = false;
        for (const auto & g : fp->raw_graphics_sexpr) {
            if (g.find("F.SilkS") != std::string::npos || g.find("B.SilkS") != std::string::npos) { has_silk = true; break; }
        }
        if (!has_silk) continue;
        for (const auto & p : fp->pads) {
            std::string layer = p.layers.empty() ? std::string() : p.layers[0];
            if (layer != "F.Cu" && layer != "B.Cu" && layer != "*.Cu") continue;
            Violation v;
            v.rule_id  = "silk_over_copper";
            v.severity = cfg.silk_over_copper;
            v.message  = "footprint " + fp->lib_id + " has silk that may overlap copper pad " + p.number;
            v.has_pos  = true;
            v.at       = world_pad_center(*fp, p);
            tally(r, std::move(v));
            break; // one report per footprint is enough
        }
    }

    return r;
}

std::string to_kicad_json(const Report & rep, std::string_view source_path) {
    std::ostringstream os;
    os << "{\n"
       << "  \"$schema\": \"kicad_drc\",\n"
       << "  \"source\": \"" << source_path << "\",\n"
       << "  \"kicad_version\": \"tool/drc\",\n"
       << "  \"coordinate_units\": \"mm\",\n"
       << "  \"violations\": [\n";
    for (std::size_t i = 0; i < rep.violations.size(); ++i) {
        const auto & v = rep.violations[i];
        os << "    {\n"
           << "      \"type\": \""     << v.rule_id  << "\",\n"
           << "      \"severity\": \"" << sev_str(v.severity) << "\",\n"
           << "      \"description\": \"" << v.message << "\",\n"
           << "      \"items\": [";
        if (v.has_pos) {
            os << "{\"description\":\"\",\"pos\":{\"x\":"
               << geom::nm_to_mm(v.at.x) << ",\"y\":"
               << geom::nm_to_mm(v.at.y) << "}}";
        }
        os << "]\n"
           << "    }" << (i + 1 < rep.violations.size() ? "," : "") << "\n";
    }
    os << "  ],\n"
       << "  \"unconnected_items\": []\n"
       << "}\n";
    return os.str();
}

} // namespace drc
