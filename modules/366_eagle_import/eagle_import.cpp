// SPDX-License-Identifier: GPL-3.0-or-later
#include "eagle_import.hpp"

#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace eagle_import {

namespace {

// Ultra-minimal XML scanner.
// Real XML parsing (namespaces, comments across CDATA, entities) is
// out of scope; EAGLE files are strictly structured and don't use
// exotic features, so a stripped-down scanner suffices.
//
// The scanner yields events (tag_open, tag_close, tag_selfclose)
// carrying (name, attrs). Text nodes are ignored (EAGLE stores
// everything in attributes).

struct Attrs {
    std::vector<std::pair<std::string, std::string>> kv;
    std::string value(std::string_view name, std::string_view dflt = {}) const {
        for (const auto & p : kv) if (p.first == name) return p.second;
        return std::string(dflt);
    }
    double dvalue(std::string_view name, double dflt = 0.0) const {
        std::string s = value(name);
        return s.empty() ? dflt : std::atof(s.c_str());
    }
    int    ivalue(std::string_view name, int dflt = 0) const {
        std::string s = value(name);
        return s.empty() ? dflt : std::atoi(s.c_str());
    }
};

struct Node {
    std::string name;
    Attrs attrs;
    std::vector<std::shared_ptr<Node>> children;

    const Node * find(std::string_view n) const {
        for (const auto & c : children) if (c->name == n) return c.get();
        return nullptr;
    }
    std::vector<const Node*> find_all(std::string_view n) const {
        std::vector<const Node*> out;
        for (const auto & c : children) if (c->name == n) out.push_back(c.get());
        return out;
    }
};

std::string unescape_entity(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '&') {
            auto sc = s.find(';', i + 1);
            if (sc != std::string_view::npos) {
                auto e = s.substr(i + 1, sc - i - 1);
                if (e == "amp")       { out += '&';  i = sc; continue; }
                if (e == "lt")        { out += '<';  i = sc; continue; }
                if (e == "gt")        { out += '>';  i = sc; continue; }
                if (e == "quot")      { out += '"';  i = sc; continue; }
                if (e == "apos")      { out += '\''; i = sc; continue; }
            }
        }
        out += s[i];
    }
    return out;
}

class Scanner {
public:
    Scanner(std::string_view text) : m_t(text), m_p(0) {}

    std::shared_ptr<Node> parse() {
        skip_prolog();
        return read_element();
    }
private:
    std::string_view m_t;
    std::size_t      m_p;

    void skip_ws() { while (m_p < m_t.size() && std::isspace(static_cast<unsigned char>(m_t[m_p]))) ++m_p; }
    void skip_prolog() {
        // Skip <?xml ...?> and <!DOCTYPE ...> and comments.
        skip_ws();
        while (m_p + 1 < m_t.size() && m_t[m_p] == '<' && (m_t[m_p+1] == '?' || m_t[m_p+1] == '!')) {
            if (m_t[m_p+1] == '?') {
                auto end = m_t.find("?>", m_p);
                if (end == std::string_view::npos) { m_p = m_t.size(); return; }
                m_p = end + 2;
            } else {
                auto end = m_t.find('>', m_p);
                if (end == std::string_view::npos) { m_p = m_t.size(); return; }
                m_p = end + 1;
            }
            skip_ws();
        }
    }
    void skip_comment() {
        // At <!-- ... -->
        auto end = m_t.find("-->", m_p);
        if (end == std::string_view::npos) { m_p = m_t.size(); return; }
        m_p = end + 3;
    }

    // Read an element starting at '<'. Returns null on error.
    std::shared_ptr<Node> read_element() {
        skip_ws();
        while (m_p + 3 < m_t.size() && m_t.substr(m_p, 4) == "<!--") { skip_comment(); skip_ws(); }
        if (m_p >= m_t.size() || m_t[m_p] != '<') return nullptr;
        ++m_p;
        auto node = std::make_shared<Node>();
        // Name.
        auto ns = m_p;
        while (m_p < m_t.size() && !std::isspace(static_cast<unsigned char>(m_t[m_p]))
                                    && m_t[m_p] != '>' && m_t[m_p] != '/') ++m_p;
        node->name = std::string(m_t.substr(ns, m_p - ns));
        // Attributes.
        for (;;) {
            skip_ws();
            if (m_p >= m_t.size()) return nullptr;
            if (m_t[m_p] == '/' || m_t[m_p] == '>') break;
            // Read name="value"
            auto as = m_p;
            while (m_p < m_t.size() && m_t[m_p] != '=' && !std::isspace(static_cast<unsigned char>(m_t[m_p]))) ++m_p;
            std::string aname(m_t.substr(as, m_p - as));
            skip_ws();
            if (m_p >= m_t.size() || m_t[m_p] != '=') break;
            ++m_p;
            skip_ws();
            char q = '"';
            if (m_p < m_t.size() && (m_t[m_p] == '"' || m_t[m_p] == '\'')) { q = m_t[m_p++]; }
            auto vs = m_p;
            while (m_p < m_t.size() && m_t[m_p] != q) ++m_p;
            std::string aval(m_t.substr(vs, m_p - vs));
            if (m_p < m_t.size()) ++m_p;
            node->attrs.kv.emplace_back(std::move(aname), unescape_entity(aval));
        }
        // Self-close?
        if (m_p < m_t.size() && m_t[m_p] == '/') {
            ++m_p;
            if (m_p < m_t.size() && m_t[m_p] == '>') ++m_p;
            return node;
        }
        // Open tag: consume '>'
        if (m_p < m_t.size() && m_t[m_p] == '>') ++m_p;

        // Children until </name>.
        std::string closer = "</" + node->name;
        for (;;) {
            skip_ws();
            while (m_p + 3 < m_t.size() && m_t.substr(m_p, 4) == "<!--") { skip_comment(); skip_ws(); }
            if (m_p >= m_t.size()) return node;
            if (m_p + closer.size() < m_t.size() && m_t.substr(m_p, closer.size()) == closer) {
                auto e = m_t.find('>', m_p);
                m_p = (e == std::string_view::npos) ? m_t.size() : e + 1;
                return node;
            }
            if (m_t[m_p] != '<') { ++m_p; continue; }   // skip text nodes
            auto child = read_element();
            if (!child) return node;
            node->children.push_back(std::move(child));
        }
    }
};

// Layer mapping.
std::string map_layer(int eagle_layer) {
    switch (eagle_layer) {
        case  1: return "F.Cu";
        case 16: return "B.Cu";
        case 20: return "Edge.Cuts";
        case 21: return "F.SilkS";
        case 22: return "B.SilkS";
        case 25: return "F.Fab";
        case 26: return "B.Fab";
        case 27: return "Dwgs.User";
        case 28: return "Dwgs.User";
        case 29: return "F.Mask";
        case 30: return "B.Mask";
        case 31: return "F.Paste";
        case 32: return "B.Paste";
        case 39: return "F.CrtYd";
        case 40: return "B.CrtYd";
        case 41: return "Eco1.User";
        case 51: return "F.Fab";
        case 52: return "B.Fab";
        default: return "Dwgs.User";
    }
}

geom::VECTOR2I xy_mm(double x_mm, double y_mm) {
    return { geom::mm_to_nm(x_mm), geom::mm_to_nm(y_mm) };
}

} // namespace

std::optional<kicad_model::Board> read_board(std::string_view xml, ImportReport * report) {
    Scanner sc(xml);
    auto root = sc.parse();
    if (!root || root->name != "eagle") {
        if (report) report->warnings = "not an eagle XML file";
        return std::nullopt;
    }
    const Node * drawing = root->find("drawing");
    if (!drawing) { if (report) report->warnings = "no <drawing>"; return std::nullopt; }
    const Node * board = drawing->find("board");
    if (!board)   { if (report) report->warnings = "no <board>";   return std::nullopt; }

    kicad_model::Board b;
    b.uuid = kicad_model::make_uuid();
    b.layers = kicad_model::default_2layer_stackup();
    kicad_model::intern_net(b, "");

    // Plain-shape blocks: <plain> holds wires/rectangles/circles on
    // non-signal layers (typically Edge.Cuts on layer 20).
    if (const Node * plain = board->find("plain")) {
        for (const auto * w : plain->find_all("wire")) {
            int layer = w->attrs.ivalue("layer", 20);
            std::string kl = map_layer(layer);
            auto g = std::make_shared<kicad_model::GrLine>();
            g->start = xy_mm(w->attrs.dvalue("x1"), w->attrs.dvalue("y1"));
            g->end   = xy_mm(w->attrs.dvalue("x2"), w->attrs.dvalue("y2"));
            g->width_nm = geom::mm_to_nm(w->attrs.dvalue("width", 0.15));
            g->layer = kl;
            g->uuid  = kicad_model::make_uuid();
            b.items.push_back(g);
            if (kl == "Edge.Cuts" && report) ++report->edge_cuts;
        }
    }

    // Placed elements (footprints).
    if (const Node * elements = board->find("elements")) {
        for (const auto * e : elements->find_all("element")) {
            auto fp = std::make_shared<kicad_model::Footprint>();
            std::string ref = e->attrs.value("name");
            std::string val = e->attrs.value("value");
            std::string lib = e->attrs.value("library");
            std::string pkg = e->attrs.value("package");
            fp->lib_id = lib + ":" + pkg;
            fp->at = xy_mm(e->attrs.dvalue("x"), e->attrs.dvalue("y"));
            // <rot> is a string like "R90" or "MR180" (mirrored).
            std::string rot = e->attrs.value("rot");
            double deg = 0.0;
            if (!rot.empty()) {
                std::size_t i = 0;
                while (i < rot.size() && !std::isdigit(static_cast<unsigned char>(rot[i])) && rot[i] != '-') ++i;
                deg = std::atof(rot.c_str() + i);
                if (rot.find('M') != std::string::npos) fp->placement_layer = "B.Cu";
            }
            fp->angle = geom::EDA_ANGLE{deg};
            fp->fields.push_back({"Reference", ref, {}, {}, false, false, false, 1, 1, {}, kicad_model::make_uuid()});
            fp->fields.push_back({"Value",     val, {}, {}, false, false, false, 1, 1, {}, kicad_model::make_uuid()});
            fp->uuid = kicad_model::make_uuid();
            b.items.push_back(fp);
            if (report) ++report->elements;
        }
    }

    // Signals: nets + tracks/vias.
    if (const Node * signals = board->find("signals")) {
        int next_net = static_cast<int>(b.nets.size());
        std::unordered_map<std::string, int> net_id;
        for (const auto * s : signals->find_all("signal")) {
            std::string name = s->attrs.value("name");
            int id = next_net++;
            b.nets.push_back({ id, name });
            net_id[name] = id;
            if (report) ++report->signals;

            for (const auto * w : s->find_all("wire")) {
                int layer = w->attrs.ivalue("layer", 1);
                std::string kl = map_layer(layer);
                if (kl != "F.Cu" && kl != "B.Cu") continue;
                auto t = std::make_shared<kicad_model::PcbTrack>();
                t->start = xy_mm(w->attrs.dvalue("x1"), w->attrs.dvalue("y1"));
                t->end   = xy_mm(w->attrs.dvalue("x2"), w->attrs.dvalue("y2"));
                t->width_nm = geom::mm_to_nm(w->attrs.dvalue("width", 0.2));
                t->layer = kl;
                t->net   = id;
                t->uuid  = kicad_model::make_uuid();
                b.items.push_back(t);
                if (report) ++report->wires;
            }
            for (const auto * v : s->find_all("via")) {
                auto pv = std::make_shared<kicad_model::PcbVia>();
                pv->at = xy_mm(v->attrs.dvalue("x"), v->attrs.dvalue("y"));
                pv->size_nm  = geom::mm_to_nm(v->attrs.dvalue("diameter", 0.6));
                pv->drill_nm = geom::mm_to_nm(v->attrs.dvalue("drill",    0.3));
                pv->net = id;
                pv->uuid = kicad_model::make_uuid();
                b.items.push_back(pv);
                if (report) ++report->vias;
            }
        }
    }

    if (report) report->ok = true;
    return b;
}

std::optional<kicad_model::Board> read_board_file(std::string_view path, ImportReport * report) {
    std::ifstream f{std::string(path)};
    if (!f) { if (report) report->warnings = "cannot open " + std::string(path); return std::nullopt; }
    std::stringstream ss; ss << f.rdbuf();
    return read_board(ss.str(), report);
}

} // namespace eagle_import
