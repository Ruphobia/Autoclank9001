// SPDX-License-Identifier: GPL-3.0-or-later
#include "pcb_layout.hpp"

#include "../341_kicad_libs/kicad_libs.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace pcb_layout {
namespace {

std::mutex g_mtx;
bool       g_ready = false;

// PCB file format version tag.
constexpr const char * KIVER = "20241229";

std::string sesc(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 2);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

std::string new_uuid() {
    static thread_local std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<std::uint64_t> dist;
    std::uint64_t a = dist(rng), b = dist(rng);
    a = (a & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    b = (b & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    char buf[37];
    std::snprintf(buf, sizeof(buf),
                  "%08lx-%04lx-%04lx-%04lx-%012lx",
                  static_cast<unsigned long>((a >> 32) & 0xFFFFFFFFULL),
                  static_cast<unsigned long>((a >> 16) & 0xFFFFULL),
                  static_cast<unsigned long>(a & 0xFFFFULL),
                  static_cast<unsigned long>((b >> 48) & 0xFFFFULL),
                  static_cast<unsigned long>(b & 0xFFFFFFFFFFFFULL));
    return std::string(buf, 36);
}

// Standard 2-layer stackup.
std::string emit_layers_2layer() {
    return
        "\t(layers\n"
        "\t\t(0 \"F.Cu\" signal)\n"
        "\t\t(2 \"B.Cu\" signal)\n"
        "\t\t(9 \"F.Adhes\" user \"F.Adhesive\")\n"
        "\t\t(11 \"B.Adhes\" user \"B.Adhesive\")\n"
        "\t\t(13 \"F.Paste\" user)\n"
        "\t\t(15 \"B.Paste\" user)\n"
        "\t\t(5 \"F.SilkS\" user \"F.Silkscreen\")\n"
        "\t\t(7 \"B.SilkS\" user \"B.Silkscreen\")\n"
        "\t\t(1 \"F.Mask\" user)\n"
        "\t\t(3 \"B.Mask\" user)\n"
        "\t\t(17 \"Dwgs.User\" user \"User.Drawings\")\n"
        "\t\t(19 \"Cmts.User\" user \"User.Comments\")\n"
        "\t\t(21 \"Eco1.User\" user \"User.Eco1\")\n"
        "\t\t(23 \"Eco2.User\" user \"User.Eco2\")\n"
        "\t\t(25 \"Edge.Cuts\" user)\n"
        "\t\t(27 \"Margin\" user)\n"
        "\t\t(31 \"F.CrtYd\" user \"F.Courtyard\")\n"
        "\t\t(29 \"B.CrtYd\" user \"B.Courtyard\")\n"
        "\t\t(35 \"F.Fab\" user)\n"
        "\t\t(33 \"B.Fab\" user)\n"
        "\t)\n";
}

std::string emit_setup(const Options & opts) {
    std::ostringstream os;
    os  << "\t(setup\n"
        << "\t\t(pad_to_mask_clearance 0.0)\n"
        << "\t\t(allow_soldermask_bridges_in_footprints no)\n"
        << "\t\t(tenting front back)\n"
        << "\t\t(covering front back)\n"
        << "\t\t(plotting_directives\n"
        << "\t\t\t(disableapertmacros no)\n"
        << "\t\t)\n"
        << "\t\t(pcbplotparams\n"
        << "\t\t\t(layerselection 0x00010fc_ffffffff)\n"
        << "\t\t\t(plot_on_all_layers_selection 0x00000000_00000000)\n"
        << "\t\t\t(disableapertmacros no)\n"
        << "\t\t\t(usegerberextensions no)\n"
        << "\t\t\t(usegerberattributes yes)\n"
        << "\t\t\t(usegerberadvancedattributes yes)\n"
        << "\t\t\t(creategerberjobfile yes)\n"
        << "\t\t\t(dashed_line_dash_ratio 12.000000)\n"
        << "\t\t\t(dashed_line_gap_ratio 3.000000)\n"
        << "\t\t\t(svgprecision 4)\n"
        << "\t\t\t(plotframeref no)\n"
        << "\t\t\t(mode 1)\n"
        << "\t\t\t(useauxorigin no)\n"
        << "\t\t\t(hpglpennumber 1)\n"
        << "\t\t\t(hpglpenspeed 20)\n"
        << "\t\t\t(hpglpendiameter 15.000000)\n"
        << "\t\t\t(pdf_front_fp_property_popups yes)\n"
        << "\t\t\t(pdf_back_fp_property_popups yes)\n"
        << "\t\t\t(pdf_metadata yes)\n"
        << "\t\t\t(pdf_single_document no)\n"
        << "\t\t\t(dxfpolygonmode yes)\n"
        << "\t\t\t(dxfimperialunits yes)\n"
        << "\t\t\t(dxfusepcbnewfont yes)\n"
        << "\t\t\t(psnegative no)\n"
        << "\t\t\t(psa4output no)\n"
        << "\t\t\t(plot_black_and_white yes)\n"
        << "\t\t\t(plotinvisibletext no)\n"
        << "\t\t\t(sketchpadsonfab no)\n"
        << "\t\t\t(plotpadnumbers no)\n"
        << "\t\t\t(hidednponfabricationlayers no)\n"
        << "\t\t\t(sketchdnponfabricationlayers yes)\n"
        << "\t\t\t(crossoutdnponfabricationlayers yes)\n"
        << "\t\t\t(subtractmaskfromsilk no)\n"
        << "\t\t\t(outputformat 1)\n"
        << "\t\t\t(mirror no)\n"
        << "\t\t\t(drillshape 1)\n"
        << "\t\t\t(scaleselection 1)\n"
        << "\t\t\t(outputdirectory \"\")\n"
        << "\t\t)\n"
        << "\t)\n";
    return os.str();
}

std::string emit_general(double w, double h) {
    std::ostringstream os;
    os << "\t(general\n"
       << "\t\t(thickness 1.6)\n"
       << "\t\t(legacy_teardrops no)\n"
       << "\t)\n"
       << "\t(paper \"A4\")\n";
    (void) w; (void) h;
    return os.str();
}

std::string emit_edge_cuts(double w, double h) {
    // Rectangle at (0,0)-(w,h) on Edge.Cuts.
    std::ostringstream os;
    auto seg = [&](double x1, double y1, double x2, double y2) {
        os << "\t(gr_line\n"
           << "\t\t(start " << x1 << " " << y1 << ")\n"
           << "\t\t(end "   << x2 << " " << y2 << ")\n"
           << "\t\t(stroke (width 0.05) (type default))\n"
           << "\t\t(layer \"Edge.Cuts\")\n"
           << "\t\t(uuid \"" << new_uuid() << "\")\n"
           << "\t)\n";
    };
    seg(0, 0, w, 0);
    seg(w, 0, w, h);
    seg(w, h, 0, h);
    seg(0, h, 0, 0);
    return os.str();
}

// Re-anchor a footprint block's (at ...) top-level to a new position
// and stamp fresh uuids so multiple placements don't collide.
std::string place_footprint(std::string fp_block,
                            double x, double y,
                            std::string_view ref,
                            std::string_view value,
                            const std::unordered_map<std::string, int> & pin_to_net) {
    // Replace the first "(at 0 0)" style top-level position. KiCad
    // footprints stored on disk always have position (0 0) or omit it;
    // we insert after the header if missing.
    std::string header_needle = "(footprint";
    std::size_t hp = fp_block.find(header_needle);
    if (hp == std::string::npos) return fp_block;

    // Locate the position insertion point: just before "(layer" if
    // present; else after the header line.
    std::size_t insert_pos = fp_block.find("(layer", hp);
    if (insert_pos == std::string::npos) {
        insert_pos = fp_block.find('\n', hp);
        if (insert_pos == std::string::npos) insert_pos = fp_block.size();
        ++insert_pos;
    }

    std::ostringstream at;
    at << "(at " << std::fixed << std::setprecision(3) << x << " " << y << " 0)\n\t";
    // Try to replace an existing (at ...) block within the first
    // ~2000 characters of the footprint; otherwise splice ours in.
    std::size_t search_end = std::min<std::size_t>(fp_block.size(), hp + 2000);
    std::size_t at_pos = fp_block.find("(at ", hp);
    if (at_pos != std::string::npos && at_pos < search_end) {
        // Skip if the "(at ...)" is inside a nested form like (pad ... (at ...)).
        // Heuristic: the top-level (at ...) appears before the first (pad ...).
        std::size_t first_pad = fp_block.find("(pad ", hp);
        if (first_pad == std::string::npos || at_pos < first_pad) {
            std::size_t close = fp_block.find(')', at_pos);
            if (close != std::string::npos) {
                fp_block.replace(at_pos, close - at_pos + 1, at.str());
            }
        } else {
            fp_block.insert(insert_pos, "\t" + at.str());
        }
    } else {
        fp_block.insert(insert_pos, "\t" + at.str());
    }

    // Stamp fresh uuids on this footprint's own uuid and pad uuids.
    // Simple pass: replace "(uuid \"xxxxxxxx-...-xxxxxxxxxxxx\")" with
    // fresh ids one at a time.
    std::string uuid_key = "(uuid \"";
    std::size_t p = 0;
    while ((p = fp_block.find(uuid_key, p)) != std::string::npos) {
        std::size_t val_start = p + uuid_key.size();
        std::size_t val_end   = fp_block.find('"', val_start);
        if (val_end == std::string::npos) break;
        fp_block.replace(val_start, val_end - val_start, new_uuid());
        p = val_start + 36;
    }

    // Bind reference / value fields.
    auto set_property = [&](const char * prop, std::string_view v) {
        std::string needle = std::string("(property \"") + prop + "\" \"";
        std::size_t start = fp_block.find(needle);
        if (start == std::string::npos) return;
        std::size_t vs = start + needle.size();
        std::size_t ve = fp_block.find('"', vs);
        if (ve == std::string::npos) return;
        fp_block.replace(vs, ve - vs, sesc(v));
    };
    set_property("Reference", ref);
    set_property("Value",     value);

    // Bind pad nets. For every (pad "N" ...) that appears without an
    // explicit (net ...) child, splice (net N "name") in from
    // pin_to_net.
    std::string pad_key = "(pad \"";
    p = 0;
    while ((p = fp_block.find(pad_key, p)) != std::string::npos) {
        std::size_t num_start = p + pad_key.size();
        std::size_t num_end   = fp_block.find('"', num_start);
        if (num_end == std::string::npos) break;
        std::string pad_num = fp_block.substr(num_start, num_end - num_start);
        // Find close paren of this (pad ...) form to constrain net insertion.
        int depth = 0;
        std::size_t pad_end = std::string::npos;
        for (std::size_t i = p; i < fp_block.size(); ++i) {
            if (fp_block[i] == '(') ++depth;
            else if (fp_block[i] == ')') { if (--depth == 0) { pad_end = i; break; } }
        }
        if (pad_end == std::string::npos) break;

        // Skip if this pad already has a (net ...) inside its block.
        std::size_t existing = fp_block.find("(net ", p);
        if (existing == std::string::npos || existing > pad_end) {
            auto it = pin_to_net.find(pad_num);
            if (it != pin_to_net.end()) {
                std::ostringstream nb;
                nb << " (net " << it->second << " \"" << sesc(std::string("net_") + std::to_string(it->second)) << "\")";
                fp_block.insert(pad_end, nb.str());
                pad_end += nb.str().size();
            }
        }
        p = pad_end + 1;
    }

    return fp_block;
}

std::string placeholder_footprint(std::string_view lib_id,
                                  double x, double y,
                                  std::string_view ref,
                                  std::string_view value) {
    std::ostringstream os;
    os << "\t(footprint \"" << sesc(lib_id) << "\"\n"
       << "\t\t(layer \"F.Cu\")\n"
       << "\t\t(uuid \"" << new_uuid() << "\")\n"
       << "\t\t(at " << std::fixed << std::setprecision(3) << x << " " << y << " 0)\n"
       << "\t\t(descr \"placeholder (not found in library index)\")\n"
       << "\t\t(attr through_hole)\n"
       << "\t\t(property \"Reference\" \"" << sesc(ref) << "\"\n"
       << "\t\t\t(at 0 -3 0)\n"
       << "\t\t\t(layer \"F.SilkS\")\n"
       << "\t\t\t(uuid \"" << new_uuid() << "\")\n"
       << "\t\t\t(effects (font (size 1 1) (thickness 0.15)))\n"
       << "\t\t)\n"
       << "\t\t(property \"Value\" \"" << sesc(value) << "\"\n"
       << "\t\t\t(at 0 3 0)\n"
       << "\t\t\t(layer \"F.Fab\")\n"
       << "\t\t\t(uuid \"" << new_uuid() << "\")\n"
       << "\t\t\t(effects (font (size 1 1) (thickness 0.15)))\n"
       << "\t\t)\n"
       << "\t\t(fp_line (start -2.5 -1.5) (end 2.5 -1.5)\n"
       << "\t\t\t(stroke (width 0.1) (type default)) (layer \"F.CrtYd\") (uuid \"" << new_uuid() << "\"))\n"
       << "\t\t(fp_line (start 2.5 -1.5) (end 2.5 1.5)\n"
       << "\t\t\t(stroke (width 0.1) (type default)) (layer \"F.CrtYd\") (uuid \"" << new_uuid() << "\"))\n"
       << "\t\t(fp_line (start 2.5 1.5) (end -2.5 1.5)\n"
       << "\t\t\t(stroke (width 0.1) (type default)) (layer \"F.CrtYd\") (uuid \"" << new_uuid() << "\"))\n"
       << "\t\t(fp_line (start -2.5 1.5) (end -2.5 -1.5)\n"
       << "\t\t\t(stroke (width 0.1) (type default)) (layer \"F.CrtYd\") (uuid \"" << new_uuid() << "\"))\n"
       << "\t\t(pad \"1\" thru_hole circle (at -1.27 0) (size 1.7 1.7) (drill 0.8) (layers \"*.Cu\" \"*.Mask\") (uuid \"" << new_uuid() << "\"))\n"
       << "\t\t(pad \"2\" thru_hole circle (at 1.27 0) (size 1.7 1.7) (drill 0.8) (layers \"*.Cu\" \"*.Mask\") (uuid \"" << new_uuid() << "\"))\n"
       << "\t)\n";
    return os.str();
}

} // namespace

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_ready) return;
    kicad_libs::init();
    g_ready = true;
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_ready = false;
}

Status status() {
    Status s;
    s.ready  = g_ready;
    s.detail = g_ready ? "pcb_layout initialized" : "not initialized";
    return s;
}

Result from_intent(const circuit_intent::Intent & intent, const Options & opts) {
    Result r;

    double w = opts.board_width_mm  > 0 ? opts.board_width_mm  : intent.board.width_mm;
    double h = opts.board_height_mm > 0 ? opts.board_height_mm : intent.board.height_mm;
    if (w <= 0) w = 50.0;
    if (h <= 0) h = 30.0;

    // 1. Build net list: 0 = "" (unconnected), then one per intent net.
    struct NetEntry { int idx; std::string name; };
    std::vector<NetEntry> netlist;
    netlist.push_back({0, ""});

    // Map "ref.pin" -> net index. Named-endpoint-only nets (no dot in
    // any endpoint) still get an index but won't bind to a pad.
    std::unordered_map<std::string, int> ep_to_net;
    for (const auto & n : intent.nets) {
        int idx = static_cast<int>(netlist.size());
        std::string net_name = n.name.empty()
            ? (std::string("Net-") + std::to_string(idx))
            : n.name;
        netlist.push_back({idx, net_name});
        for (const auto & e : n.endpoints) {
            ep_to_net[e.endpoint] = idx;
        }
    }
    r.emitted_nets = static_cast<int>(netlist.size());

    // 2. Assemble the file.
    std::ostringstream out;
    out << "(kicad_pcb\n"
        << "\t(version " << KIVER << ")\n"
        << "\t(generator \"" << sesc(opts.generator) << "\")\n"
        << "\t(generator_version \"" << sesc(opts.generator_version) << "\")\n";

    out << emit_general(w, h);
    out << emit_layers_2layer();
    out << emit_setup(opts);

    for (const auto & n : netlist) {
        out << "\t(net " << n.idx << " \"" << sesc(n.name) << "\")\n";
    }

    // Footprints.
    int placed = 0;
    for (const auto & p : intent.parts) {
        int cols = opts.columns > 0 ? opts.columns : 4;
        int col = placed % cols;
        int row = placed / cols;
        double x = opts.origin_x_mm + col * opts.grid_pitch_mm;
        double y = opts.origin_y_mm + row * opts.grid_pitch_mm;
        ++placed;

        std::unordered_map<std::string, int> pin_to_net;
        for (const auto & kv : ep_to_net) {
            const auto & ep = kv.first;
            auto dot = ep.find('.');
            if (dot == std::string::npos) continue;
            if (ep.substr(0, dot) != p.ref) continue;
            pin_to_net[ep.substr(dot + 1)] = kv.second;
        }

        std::string block;
        if (!p.footprint_hint.empty()) {
            block = kicad_libs::extract_footprint_block(p.footprint_hint);
        }
        if (block.empty()) {
            out << placeholder_footprint(p.footprint_hint.empty() ? std::string("tool:placeholder") : p.footprint_hint,
                                         x, y, p.ref, p.value);
            ++r.placeholder_footprints;
            r.diagnostics.push_back({
                circuit_intent::Diagnostic::Severity::Warning,
                "footprints",
                "footprint '" + p.footprint_hint + "' not found; placeholder used for " + p.ref
            });
        } else {
            out << "\t" << place_footprint(std::move(block), x, y, p.ref, p.value, pin_to_net) << "\n";
            ++r.resolved_footprints;
        }
    }

    out << emit_edge_cuts(w, h);
    out << "\t(embedded_fonts no)\n"
        << ")\n";

    r.pcb_text = out.str();
    r.ok = true;
    return r;
}

bool write_file(const std::string & path, const Result & r) {
    if (!r.ok || r.pcb_text.empty()) return false;
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    f.write(r.pcb_text.data(), static_cast<std::streamsize>(r.pcb_text.size()));
    return static_cast<bool>(f);
}

} // namespace pcb_layout
