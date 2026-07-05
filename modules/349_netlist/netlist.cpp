// SPDX-License-Identifier: GPL-3.0-or-later
#include "netlist.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace netlist {

namespace {

using kicad_model::Schematic;
using kicad_model::SchScreen;
using kicad_model::SchSymbol;
using kicad_model::SchWire;
using kicad_model::SchJunction;
using kicad_model::SchNoConnect;
using kicad_model::SchLabel;
using kicad_model::SchGlobalLabel;
using kicad_model::SchHierLabel;
using kicad_model::LibSymbol;
using kicad_model::SchPin;
using kicad_model::ItemType;
using geom::VECTOR2I;

// A grid tolerance for point-equality. KiCad uses 1 mil (25400 nm) or
// smaller for schematic grid; we snap to 1000 nm (1 um) which is
// finer than any real drawing.
constexpr long long SNAP_NM = 1000;

struct PointKey {
    long long x = 0, y = 0;
    bool operator==(const PointKey & o) const { return x == o.x && y == o.y; }
};

struct PointKeyHash {
    std::size_t operator()(const PointKey & k) const noexcept {
        return std::hash<long long>{}(k.x) ^ (std::hash<long long>{}(k.y) << 1);
    }
};

PointKey snap(VECTOR2I v) {
    auto snap1 = [](long long z) -> long long {
        long long r = z / SNAP_NM;
        return r * SNAP_NM;
    };
    return { snap1(v.x), snap1(v.y) };
}

// Union-find over integer node ids.
struct UF {
    std::vector<int> parent;
    int add() { int id = static_cast<int>(parent.size()); parent.push_back(id); return id; }
    int root(int x) {
        while (parent[x] != x) { parent[x] = parent[parent[x]]; x = parent[x]; }
        return x;
    }
    void unite(int a, int b) {
        int ra = root(a), rb = root(b);
        if (ra != rb) parent[ra] = rb;
    }
};

// Rotate a pin's local (x,y) around the symbol origin by the symbol's
// angle. KiCad's symbol coordinate system has +Y down in the file
// but +Y up in the model; we don't flip because both sides of the
// pipeline use the same convention.
VECTOR2I world_pin_pos(const SchSymbol & sym, const SchPin & pin) {
    // Apply mirror.
    long long lx = pin.at.x, ly = pin.at.y;
    if (sym.mirror_x) ly = -ly;
    if (sym.mirror_y) lx = -lx;
    // Rotate by symbol angle.
    double a = sym.angle.rad();
    double c = std::cos(a), s = std::sin(a);
    double rx = lx * c - ly * s;
    double ry = lx * s + ly * c;
    return { sym.at.x + static_cast<long long>(std::llround(rx)),
             sym.at.y + static_cast<long long>(std::llround(ry)) };
}

// Does point p lie on the interior of a wire segment (a,b)?
bool on_segment(VECTOR2I p, VECTOR2I a, VECTOR2I b, long long tol = SNAP_NM) {
    // Vector cross of (b-a) x (p-a) must be ~0 (colinear).
    long long dx1 = b.x - a.x, dy1 = b.y - a.y;
    long long dx2 = p.x - a.x, dy2 = p.y - a.y;
    // Overflow-safe cross via double.
    double cross = static_cast<double>(dx1) * dy2 - static_cast<double>(dy1) * dx2;
    if (std::abs(cross) > static_cast<double>(tol) * std::hypot(static_cast<double>(dx1), static_cast<double>(dy1)))
        return false;
    // Check p lies between a and b via dot product.
    double t;
    if (std::abs(dx1) >= std::abs(dy1)) {
        if (dx1 == 0) return p == a;
        t = static_cast<double>(dx2) / static_cast<double>(dx1);
    } else {
        if (dy1 == 0) return p == a;
        t = static_cast<double>(dy2) / static_cast<double>(dy1);
    }
    return t >= 0.0 && t <= 1.0;
}

std::string synthesize_name(const Net & n) {
    if (!n.pins.empty()) {
        // Match KiCad's "Net-(U1-Pad2)" style.
        return "Net-(" + n.pins.front().ref + "-Pad" + n.pins.front().number + ")";
    }
    return "unconnected-" + std::to_string(n.id);
}

// Priority order for label kinds: global > hier > local.
// KiCad picks the highest-priority label attached to a net as its name.
int label_priority(const std::string & type) {
    if (type == "global") return 3;
    if (type == "hier")   return 2;
    if (type == "local")  return 1;
    return 0;
}

} // namespace

Netlist derive(const Schematic & sch) {
    Netlist out;
    UF uf;

    // Node registry: point-key -> node id in UF.
    std::unordered_map<PointKey, int, PointKeyHash> node_at;

    auto node_for = [&](VECTOR2I v) -> int {
        auto key = snap(v);
        auto it = node_at.find(key);
        if (it != node_at.end()) return it->second;
        int id = uf.add();
        node_at.emplace(key, id);
        return id;
    };

    // Node -> pins attached.
    std::unordered_map<int, std::vector<Pin>> node_pins;

    // Node -> highest-priority label name.
    struct LabelInfo { std::string name; int priority = 0; };
    std::unordered_map<int, LabelInfo> node_label;

    // No-connect points: pins here are considered intentionally unconnected.
    std::unordered_map<PointKey, bool, PointKeyHash> no_connect;

    // Collect wires first so we can register their endpoints.
    struct WireRef { VECTOR2I a, b; };
    std::vector<WireRef> wires;

    // Junctions.
    std::vector<VECTOR2I> junctions;

    // Symbols with their world-space pin points.
    struct SymPin { std::string ref, number, electrical; VECTOR2I pos; };
    std::vector<SymPin> sym_pins;

    for (const auto & it : sch.root.items) {
        switch (it->type) {
            case ItemType::SchWire: {
                const auto * w = static_cast<const SchWire *>(it.get());
                for (std::size_t i = 0; i + 1 < w->pts.size(); ++i)
                    wires.push_back({ w->pts[i], w->pts[i+1] });
                break;
            }
            case ItemType::SchJunction: {
                junctions.push_back(static_cast<const SchJunction *>(it.get())->at);
                break;
            }
            case ItemType::SchNoConnect: {
                no_connect.emplace(snap(static_cast<const SchNoConnect *>(it.get())->at), true);
                break;
            }
            case ItemType::SchLabel: {
                const auto * l = static_cast<const SchLabel *>(it.get());
                int n = node_for(l->at);
                auto & lbl = node_label[n];
                if (label_priority("local") > lbl.priority) { lbl.priority = label_priority("local"); lbl.name = l->text; }
                break;
            }
            case ItemType::SchGlobalLabel: {
                const auto * l = static_cast<const SchGlobalLabel *>(it.get());
                int n = node_for(l->at);
                auto & lbl = node_label[n];
                if (label_priority("global") > lbl.priority) { lbl.priority = label_priority("global"); lbl.name = l->text; }
                break;
            }
            case ItemType::SchHierLabel: {
                const auto * l = static_cast<const SchHierLabel *>(it.get());
                int n = node_for(l->at);
                auto & lbl = node_label[n];
                if (label_priority("hier") > lbl.priority) { lbl.priority = label_priority("hier"); lbl.name = l->text; }
                break;
            }
            case ItemType::SchSymbol: {
                const auto * s = static_cast<const SchSymbol *>(it.get());
                auto lib_it = sch.lib_symbols.find(s->lib_id);
                if (lib_it == sch.lib_symbols.end()) {
                    out.warnings.push_back("no lib_symbol for " + s->lib_id + " (" + s->reference() + ")");
                    break;
                }
                for (const auto & p : lib_it->second.pins) {
                    SymPin sp;
                    sp.ref        = s->reference().empty() ? std::string("?") : s->reference();
                    sp.number     = p.number;
                    sp.electrical = p.electrical.empty() ? std::string("passive") : p.electrical;
                    sp.pos        = world_pin_pos(*s, p);
                    sym_pins.push_back(std::move(sp));
                }
                break;
            }
            default: break;
        }
    }

    // Register wire endpoints and unite them per wire.
    for (const auto & w : wires) {
        int a = node_for(w.a);
        int b = node_for(w.b);
        uf.unite(a, b);
    }

    // Register pins and unite with any existing endpoint at the same position.
    for (const auto & sp : sym_pins) {
        int p = node_for(sp.pos);
        node_pins[p].push_back({ sp.ref, sp.number, sp.electrical });
    }

    // A pin can land in the middle of a wire (T-junction). Unite each
    // pin node with any wire whose segment passes through it.
    for (const auto & sp : sym_pins) {
        int p_node = node_for(sp.pos);
        for (const auto & w : wires) {
            if (on_segment(sp.pos, w.a, w.b)) {
                uf.unite(p_node, node_for(w.a));
            }
        }
    }

    // Junctions merge everything at their point.
    for (const auto & j : junctions) {
        int j_node = node_for(j);
        // Any wire passing through this junction gets merged.
        for (const auto & w : wires) {
            if (on_segment(j, w.a, w.b))
                uf.unite(j_node, node_for(w.a));
        }
    }

    // Aggregate nodes by root.
    std::unordered_map<int, Net> by_root;
    for (auto & kv : node_pins) {
        int r = uf.root(kv.first);
        auto & n = by_root[r];
        for (auto & p : kv.second) n.pins.push_back(std::move(p));
    }
    for (auto & kv : node_label) {
        int r = uf.root(kv.first);
        auto & n = by_root[r];
        // Highest priority wins.
        LabelInfo & incoming = kv.second;
        // We don't have the previous priority here; just accept the last.
        if (n.name.empty() || incoming.priority >= label_priority("local"))
            n.name = incoming.name;
    }

    // Assign ids. Net 0 is "no-connect".
    Net nc; nc.id = 0; nc.name = "";
    out.nets.push_back(nc);
    int next_id = 1;

    // Deterministic order: sort roots by their first pin's ref/pin for stability.
    std::vector<std::pair<int, Net>> ordered(by_root.begin(), by_root.end());
    std::sort(ordered.begin(), ordered.end(),
              [](const auto & a, const auto & b) {
                  auto key = [](const Net & n) -> std::string {
                      if (!n.name.empty()) return "0" + n.name;
                      if (n.pins.empty())  return "9";
                      return "1" + n.pins.front().ref + "." + n.pins.front().number;
                  };
                  return key(a.second) < key(b.second);
              });

    for (auto & kv : ordered) {
        Net & n = kv.second;
        n.id = next_id++;
        if (n.name.empty()) n.name = synthesize_name(n);
        out.nets.push_back(std::move(n));
    }

    // Route "no-connect" pins to net 0.
    for (auto & kv : node_pins) {
        auto key_it = std::find_if(node_at.begin(), node_at.end(),
            [&](const auto & p) { return p.second == kv.first; });
        if (key_it == node_at.end()) continue;
        if (!no_connect.count(key_it->first)) continue;
        for (auto & p : kv.second) out.nets[0].pins.push_back(p);
    }

    return out;
}

std::string to_kicad_netlist(const Netlist & nl, const Schematic & sch) {
    std::ostringstream os;
    os << "(export (version \"E\")\n"
       << "\t(design\n"
       << "\t\t(source \"" << sch.uuid << "\")\n"
       << "\t\t(date \"generated by tool\")\n"
       << "\t\t(tool \"tool 0.1\")\n"
       << "\t)\n";

    // Components section.
    os << "\t(components\n";
    for (const auto & it : sch.root.items) {
        if (it->type != ItemType::SchSymbol) continue;
        const auto * s = static_cast<const SchSymbol *>(it.get());
        std::string ref  = s->reference();
        std::string val  = s->value();
        std::string fp   = s->field_value("Footprint");
        std::string ds   = s->field_value("Datasheet");
        os << "\t\t(comp (ref \"" << ref << "\")\n"
           << "\t\t\t(value \"" << val << "\")\n";
        if (!fp.empty()) os << "\t\t\t(footprint \"" << fp << "\")\n";
        if (!ds.empty()) os << "\t\t\t(datasheet \"" << ds << "\")\n";
        os << "\t\t\t(libsource (lib \"" << s->lib_id.substr(0, s->lib_id.find(':'))
                                  << "\") (part \"" << s->lib_id.substr(s->lib_id.find(':')+1)
                                  << "\") (description \"\"))\n"
           << "\t\t)\n";
    }
    os << "\t)\n";

    // Nets section.
    os << "\t(nets\n";
    for (const auto & n : nl.nets) {
        os << "\t\t(net (code \"" << n.id << "\") (name \"" << n.name << "\")\n";
        for (const auto & p : n.pins) {
            os << "\t\t\t(node (ref \"" << p.ref
               << "\") (pin \""  << p.number
               << "\") (pinfunction \"" << p.electrical << "\") (pintype \"" << p.electrical << "\"))\n";
        }
        os << "\t\t)\n";
    }
    os << "\t)\n)\n";
    return os.str();
}

std::string to_spice_netlist(const Netlist & nl, const Schematic & sch,
                             std::string_view analysis) {
    std::ostringstream os;
    os << ".title tool-generated SPICE netlist for " << sch.uuid << "\n";

    // Emit one card per SchSymbol. SPICE prefixes:
    //   R for resistors (Reference starting with R)
    //   C for caps      (starting with C)
    //   L for inductors (starting with L)
    //   V for voltage sources (starting with V)
    //   I for current sources (starting with I)
    //   Q for BJT       (starting with Q)
    //   M for MOSFET    (starting with M)
    //   D for diode     (starting with D)
    //   X for subckt    (starting with U, X, IC)
    auto ref_to_prefix = [](char c) -> char {
        switch (c) {
            case 'R': case 'C': case 'L': case 'V': case 'I':
            case 'Q': case 'M': case 'D': return c;
            case 'U': case 'X': case 'K': return 'X';
            default: return 'X';
        }
    };

    // Build a per-part pin->net map.
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> by_ref;
    for (const auto & n : nl.nets) {
        for (const auto & p : n.pins)
            by_ref[p.ref].push_back({ p.number, n.name });
    }

    for (const auto & it : sch.root.items) {
        if (it->type != ItemType::SchSymbol) continue;
        const auto * s = static_cast<const SchSymbol *>(it.get());
        std::string ref = s->reference();
        if (ref.empty()) continue;
        char prefix = ref_to_prefix(ref[0]);
        // Order pins by number ascending; SPICE cards typically list pins
        // in canonical order per device kind, and this at least is stable.
        auto & pins = by_ref[ref];
        std::sort(pins.begin(), pins.end(),
                  [](const auto & a, const auto & b) { return a.first < b.first; });
        os << prefix << ref.substr(1) << ' ';
        for (const auto & p : pins) os << p.second << ' ';
        os << s->value() << "\n";
    }
    if (!analysis.empty()) {
        os << "." << analysis << "\n";
    }
    os << ".end\n";
    return os.str();
}

} // namespace netlist
