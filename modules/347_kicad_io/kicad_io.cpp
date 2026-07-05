// SPDX-License-Identifier: GPL-3.0-or-later
#include "kicad_io.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace kicad_io {

using kicad_model::UUID;
using kicad_model::Field;
using kicad_model::Schematic;
using kicad_model::SchScreen;
using kicad_model::SchSymbol;
using kicad_model::SchWire;
using kicad_model::SchBus;
using kicad_model::SchJunction;
using kicad_model::SchNoConnect;
using kicad_model::SchBusEntry;
using kicad_model::SchLabel;
using kicad_model::SchGlobalLabel;
using kicad_model::SchHierLabel;
using kicad_model::SchText;
using kicad_model::SchTextBox;
using kicad_model::SchShape;
using kicad_model::SchSheet;
using kicad_model::SchSheetPin;
using kicad_model::LibSymbol;
using kicad_model::SchPin;
using kicad_model::Board;
using kicad_model::LayerInfo;
using kicad_model::NetInfo;
using kicad_model::Footprint;
using kicad_model::Pad;
using kicad_model::PcbTrack;
using kicad_model::PcbArc;
using kicad_model::PcbVia;
using kicad_model::Zone;
using kicad_model::GrLine;
using kicad_model::GrArc;
using kicad_model::GrCircle;
using kicad_model::GrPolygon;
using kicad_model::GrText;
using kicad_model::ItemPtr;

using geom::VECTOR2I;
using geom::EDA_ANGLE;
using geom::mm_to_nm;
using geom::nm_to_mm;
using geom::format_mm;

using sexpr::SExpr;
using sexpr::SExprPtr;

// =========================================================================
// Read helpers
// =========================================================================
namespace {

std::string slurp(std::string_view path) {
    std::ifstream f{std::string(path), std::ios::binary};
    if (!f) return {};
    std::stringstream ss; ss << f.rdbuf();
    return ss.str();
}

// Extract a child list by head. Returns nullptr if missing.
SExprPtr find(const SExpr & node, std::string_view head) {
    return node.find(head);
}

// Read a string field value from either a String or Atom child at index.
std::string as_str(const SExprPtr & n) {
    if (!n) return {};
    if (n->is_string()) return n->string();
    if (n->is_atom())   return n->atom();
    if (n->is_number()) return n->number();
    return {};
}

// (uuid "xxxxx")
UUID parse_uuid(const SExpr & node) {
    auto u = node.find("uuid");
    if (!u || u->size() < 2) return {};
    return as_str(u->list()[1]);
}

// Read positional arg (child_after_head).
double num_at(const SExpr & node, std::size_t i, double dflt = 0.0) {
    auto c = node.child_after_head(i);
    if (!c || !c->is_number()) return dflt;
    return c->as_double();
}

std::string str_at(const SExpr & node, std::size_t i) {
    auto c = node.child_after_head(i);
    return as_str(c);
}

VECTOR2I parse_at(const SExpr & at_node) {
    // (at X Y [R])
    return { mm_to_nm(num_at(at_node, 0)),
             mm_to_nm(num_at(at_node, 1)) };
}
EDA_ANGLE parse_at_angle(const SExpr & at_node) {
    return EDA_ANGLE{num_at(at_node, 2, 0.0)};
}

// (pts (xy X Y) (xy X Y) ...)
std::vector<VECTOR2I> parse_pts(const SExpr & pts_node) {
    std::vector<VECTOR2I> out;
    for (const auto & c : pts_node.list()) {
        if (!c->is_list() || c->head() != "xy") continue;
        out.push_back({ mm_to_nm(num_at(*c, 0)), mm_to_nm(num_at(*c, 1)) });
    }
    return out;
}

// (effects (font (size H V) (thickness T) (bold) (italic)) (justify ...) (hide yes))
void parse_effects(const SExpr & eff, Field & f) {
    if (auto font = eff.find("font")) {
        if (auto sz = font->find("size")) {
            f.font_h_mm = num_at(*sz, 0, 1.27);
            f.font_v_mm = num_at(*sz, 1, 1.27);
        }
        if (font->find("bold"))   f.bold   = true;
        if (font->find("italic")) f.italic = true;
    }
    if (auto j = eff.find("justify")) {
        for (std::size_t i = 1; i < j->size(); ++i) {
            if (i > 1) f.justify += ' ';
            f.justify += as_str(j->list()[i]);
        }
    }
    if (auto h = eff.find("hide")) {
        // Older files: (hide) with no arg; newer: (hide yes|no).
        if (h->size() > 1) f.hide = (str_at(*h, 0) == "yes");
        else               f.hide = true;
    }
}

// Full (property "Name" "Value" (at ...) (effects ...)) parse.
Field parse_property(const SExpr & prop) {
    Field f;
    if (prop.size() >= 2) f.name  = as_str(prop.list()[1]);
    if (prop.size() >= 3) f.value = as_str(prop.list()[2]);
    if (auto at = prop.find("at")) {
        f.at    = parse_at(*at);
        f.angle = parse_at_angle(*at);
    }
    if (auto eff = prop.find("effects")) parse_effects(*eff, f);
    f.uuid = parse_uuid(prop);
    return f;
}

std::vector<Field> parse_property_list(const SExpr & node) {
    std::vector<Field> out;
    for (const auto & c : node.list()) {
        if (c->is_list() && c->head() == "property") out.push_back(parse_property(*c));
    }
    return out;
}

// (stroke (width W) (type T) (color R G B A))
void parse_stroke(const SExpr & s, double & width_mm, std::string & type, std::string & color) {
    if (auto w = s.find("width")) width_mm = num_at(*w, 0);
    if (auto t = s.find("type"))  type     = str_at(*t, 0);
    if (auto c = s.find("color")) {
        std::ostringstream os;
        os << "rgba("
           << num_at(*c, 0, 0) << ","
           << num_at(*c, 1, 0) << ","
           << num_at(*c, 2, 0) << ","
           << num_at(*c, 3, 0) << ")";
        color = os.str();
    }
}

// (fill (type solid|background|none|color) (color R G B A))
void parse_fill(const SExpr & f, std::string & type, std::string & color) {
    if (auto t = f.find("type"))  type  = str_at(*t, 0);
    if (auto c = f.find("color")) {
        std::ostringstream os;
        os << "rgba("
           << num_at(*c, 0, 0) << ","
           << num_at(*c, 1, 0) << ","
           << num_at(*c, 2, 0) << ","
           << num_at(*c, 3, 0) << ")";
        color = os.str();
    }
}

// -------------------- LibSymbol --------------------

LibSymbol parse_lib_symbol(const SExpr & sym) {
    LibSymbol s;
    if (sym.size() >= 2) s.lib_id = as_str(sym.list()[1]);
    if (sym.find("power"))            s.power = true;
    if (auto e = sym.find("extends")) s.extends = str_at(*e, 0);
    if (auto x = sym.find("exclude_from_sim")) s.exclude_from_sim = (str_at(*x, 0) == "yes");
    if (auto x = sym.find("in_bom"))           s.in_bom          = (str_at(*x, 0) == "yes");
    if (auto x = sym.find("on_board"))         s.on_board        = (str_at(*x, 0) == "yes");
    s.fields = parse_property_list(sym);

    // Pins: KiCad emits (pin electrical shape (at X Y R) (length L) (name "n") (number "1") (uuid "..."))
    for (const auto & c : sym.list()) {
        if (!c->is_list()) continue;
        if (c->head() == "pin") {
            SchPin p;
            if (c->size() >= 2) p.electrical = as_str(c->list()[1]);
            if (c->size() >= 3) p.shape      = as_str(c->list()[2]);
            if (auto at = c->find("at")) {
                p.at    = parse_at(*at);
                p.angle = parse_at_angle(*at);
            }
            if (auto L  = c->find("length")) p.length_mm = num_at(*L, 0, 2.54);
            if (auto n  = c->find("name"))   p.name      = str_at(*n, 0);
            if (auto n  = c->find("number")) p.number    = str_at(*n, 0);
            p.uuid = parse_uuid(*c);
            s.pins.push_back(std::move(p));
        }
        // Graphic children retained as raw sexpr text for now.
        else if (c->head() == "rectangle" || c->head() == "polyline" ||
                 c->head() == "circle"    || c->head() == "arc"       ||
                 c->head() == "text"      || c->head() == "bezier"    ||
                 c->head() == "text_box") {
            s.raw_graphics_sexpr.push_back(sexpr::to_kicad_string(*c));
        }
        else if (c->head() == "symbol") {
            // Nested alternate-unit symbol: preserve raw.
            s.raw_unit_sexpr.push_back(sexpr::to_kicad_string(*c));
        }
    }
    return s;
}

// -------------------- Schematic items --------------------

ItemPtr parse_sch_symbol(const SExpr & node) {
    auto s = std::make_shared<SchSymbol>();
    if (auto id = node.find("lib_id")) s->lib_id = str_at(*id, 0);
    if (auto at = node.find("at")) {
        s->at    = parse_at(*at);
        s->angle = parse_at_angle(*at);
    }
    if (auto u = node.find("unit"))       s->unit     = static_cast<int>(num_at(*u, 0, 1));
    if (auto d = node.find("dnp"))        s->dnp      = (str_at(*d, 0) == "yes");
    if (auto x = node.find("in_bom"))     s->in_bom   = (str_at(*x, 0) == "yes");
    if (auto x = node.find("on_board"))   s->on_board = (str_at(*x, 0) == "yes");
    if (auto m = node.find("mirror")) {
        std::string axis = str_at(*m, 0);
        if (axis == "x") s->mirror_x = true;
        if (axis == "y") s->mirror_y = true;
    }
    s->fields = parse_property_list(node);
    s->uuid   = parse_uuid(node);
    return s;
}

ItemPtr parse_sch_wire(const SExpr & node) {
    auto w = std::make_shared<SchWire>();
    if (auto pts = node.find("pts")) w->pts = parse_pts(*pts);
    if (auto s = node.find("stroke")) parse_stroke(*s, w->stroke_mm, w->stroke_type, w->stroke_color);
    w->uuid = parse_uuid(node);
    return w;
}
ItemPtr parse_sch_bus(const SExpr & node) {
    auto b = std::make_shared<SchBus>();
    if (auto pts = node.find("pts")) b->pts = parse_pts(*pts);
    if (auto s = node.find("stroke")) parse_stroke(*s, b->stroke_mm, b->stroke_type, b->stroke_color);
    b->uuid = parse_uuid(node);
    return b;
}
ItemPtr parse_junction(const SExpr & node) {
    auto j = std::make_shared<SchJunction>();
    if (auto at = node.find("at")) j->at = parse_at(*at);
    if (auto d  = node.find("diameter")) j->diameter_mm = num_at(*d, 0);
    j->uuid = parse_uuid(node);
    return j;
}
ItemPtr parse_no_connect(const SExpr & node) {
    auto nc = std::make_shared<SchNoConnect>();
    if (auto at = node.find("at")) nc->at = parse_at(*at);
    nc->uuid = parse_uuid(node);
    return nc;
}
ItemPtr parse_bus_entry(const SExpr & node) {
    auto e = std::make_shared<SchBusEntry>();
    if (auto at = node.find("at")) e->at = parse_at(*at);
    if (auto sz = node.find("size")) e->size = { mm_to_nm(num_at(*sz, 0)),
                                                 mm_to_nm(num_at(*sz, 1)) };
    if (auto s = node.find("stroke")) parse_stroke(*s, e->stroke_mm, e->stroke_type, {});
    e->uuid = parse_uuid(node);
    return e;
}

template <typename L>
std::shared_ptr<L> parse_label_common(const SExpr & node) {
    auto l = std::make_shared<L>();
    if (node.size() >= 2) l->text = as_str(node.list()[1]);
    if (auto at = node.find("at")) {
        l->at    = parse_at(*at);
        l->angle = parse_at_angle(*at);
    }
    if (auto sh = node.find("shape")) l->shape = str_at(*sh, 0);
    l->fields = parse_property_list(node);
    l->uuid   = parse_uuid(node);
    return l;
}

ItemPtr parse_sch_text(const SExpr & node) {
    auto t = std::make_shared<SchText>();
    if (node.size() >= 2) t->text = as_str(node.list()[1]);
    if (auto at = node.find("at")) {
        t->at    = parse_at(*at);
        t->angle = parse_at_angle(*at);
    }
    if (auto eff = node.find("effects")) {
        Field tmp;
        parse_effects(*eff, tmp);
        t->font_h_mm = tmp.font_h_mm;
        t->font_v_mm = tmp.font_v_mm;
        t->bold      = tmp.bold;
        t->italic    = tmp.italic;
        t->justify   = tmp.justify;
    }
    t->uuid = parse_uuid(node);
    return t;
}

ItemPtr parse_sch_textbox(const SExpr & node) {
    auto tb = std::make_shared<SchTextBox>();
    if (node.size() >= 2) tb->text = as_str(node.list()[1]);
    if (auto at = node.find("at")) {
        tb->at    = parse_at(*at);
        tb->angle = parse_at_angle(*at);
    }
    if (auto sz = node.find("size")) tb->size = { mm_to_nm(num_at(*sz, 0)),
                                                  mm_to_nm(num_at(*sz, 1)) };
    if (auto s  = node.find("stroke")) parse_stroke(*s, tb->stroke_mm, tb->stroke_type, tb->stroke_color);
    if (auto f  = node.find("fill"))   parse_fill(*f, tb->fill_type, tb->fill_color);
    if (auto eff = node.find("effects")) {
        Field tmp; parse_effects(*eff, tmp);
        tb->font_h_mm = tmp.font_h_mm;
        tb->font_v_mm = tmp.font_v_mm;
    }
    tb->uuid = parse_uuid(node);
    return tb;
}

ItemPtr parse_sch_shape(const SExpr & node, const std::string & kind) {
    auto s = std::make_shared<SchShape>();
    s->shape = kind;
    if (kind == "polyline" || kind == "bezier") {
        if (auto pts = node.find("pts")) s->pts = parse_pts(*pts);
    } else if (kind == "rectangle") {
        if (auto a = node.find("start")) s->start = { mm_to_nm(num_at(*a, 0)), mm_to_nm(num_at(*a, 1)) };
        if (auto b = node.find("end"))   s->end   = { mm_to_nm(num_at(*b, 0)), mm_to_nm(num_at(*b, 1)) };
    } else if (kind == "circle") {
        if (auto c = node.find("center")) s->center = { mm_to_nm(num_at(*c, 0)), mm_to_nm(num_at(*c, 1)) };
        if (auto r = node.find("radius")) s->radius_nm = mm_to_nm(num_at(*r, 0));
    } else if (kind == "arc") {
        if (auto a = node.find("start")) s->start = { mm_to_nm(num_at(*a, 0)), mm_to_nm(num_at(*a, 1)) };
        if (auto m = node.find("mid"))   s->mid   = { mm_to_nm(num_at(*m, 0)), mm_to_nm(num_at(*m, 1)) };
        if (auto e = node.find("end"))   s->end   = { mm_to_nm(num_at(*e, 0)), mm_to_nm(num_at(*e, 1)) };
    }
    if (auto st = node.find("stroke")) parse_stroke(*st, s->stroke_mm, s->stroke_type, s->stroke_color);
    if (auto fi = node.find("fill"))   parse_fill(*fi, s->fill_type, s->fill_color);
    s->uuid = parse_uuid(node);
    return s;
}

ItemPtr parse_sch_sheet(const SExpr & node) {
    auto sh = std::make_shared<SchSheet>();
    if (auto at = node.find("at")) sh->at = parse_at(*at);
    if (auto sz = node.find("size")) sh->size = { mm_to_nm(num_at(*sz, 0)),
                                                  mm_to_nm(num_at(*sz, 1)) };
    if (auto s = node.find("stroke")) parse_stroke(*s, sh->stroke_mm, sh->stroke_color, {});
    if (auto f = node.find("fill"))   parse_fill(*f, sh->fill_type, {});
    sh->fields = parse_property_list(node);
    for (const auto & c : node.list()) {
        if (!c->is_list() || c->head() != "pin") continue;
        SchSheetPin p;
        if (c->size() >= 2) p.name  = as_str(c->list()[1]);
        if (c->size() >= 3) p.shape = as_str(c->list()[2]);
        if (auto at = c->find("at")) {
            p.at    = parse_at(*at);
            p.angle = parse_at_angle(*at);
        }
        p.uuid = parse_uuid(*c);
        sh->pins.push_back(std::move(p));
    }
    sh->uuid = parse_uuid(node);
    for (const auto & f : sh->fields) {
        if (f.name == "Sheetname") sh->name      = f.value;
        if (f.name == "Sheetfile") sh->file_name = f.value;
    }
    return sh;
}

void parse_sch_children(const SExpr & node, SchScreen & scr) {
    for (const auto & c : node.list()) {
        if (!c->is_list()) continue;
        const std::string h = c->head();
        if      (h == "symbol")        scr.items.push_back(parse_sch_symbol(*c));
        else if (h == "wire")          scr.items.push_back(parse_sch_wire(*c));
        else if (h == "bus")           scr.items.push_back(parse_sch_bus(*c));
        else if (h == "junction")      scr.items.push_back(parse_junction(*c));
        else if (h == "no_connect")    scr.items.push_back(parse_no_connect(*c));
        else if (h == "bus_entry")     scr.items.push_back(parse_bus_entry(*c));
        else if (h == "label")         scr.items.push_back(parse_label_common<SchLabel>      (*c));
        else if (h == "global_label")  scr.items.push_back(parse_label_common<SchGlobalLabel>(*c));
        else if (h == "hierarchical_label") scr.items.push_back(parse_label_common<SchHierLabel>(*c));
        else if (h == "text")          scr.items.push_back(parse_sch_text(*c));
        else if (h == "text_box")      scr.items.push_back(parse_sch_textbox(*c));
        else if (h == "polyline" || h == "rectangle" || h == "circle" ||
                 h == "arc"      || h == "bezier")
                                       scr.items.push_back(parse_sch_shape(*c, h));
        else if (h == "sheet")         scr.items.push_back(parse_sch_sheet(*c));
    }
}

// -------------------- Board items --------------------

Pad parse_pad(const SExpr & node) {
    Pad p;
    if (node.size() >= 2) p.number = as_str(node.list()[1]);
    if (node.size() >= 3) p.kind   = as_str(node.list()[2]);
    if (node.size() >= 4) p.shape  = as_str(node.list()[3]);
    if (auto at = node.find("at")) {
        p.at    = parse_at(*at);
        p.angle = parse_at_angle(*at);
    }
    if (auto sz = node.find("size"))
        p.size = { mm_to_nm(num_at(*sz, 0)), mm_to_nm(num_at(*sz, 1)) };
    if (auto d = node.find("drill")) {
        // (drill D) or (drill oval Dx Dy)
        if (d->size() >= 2 && d->list()[1]->is_atom() && d->list()[1]->atom() == "oval") {
            p.drill_nm      = mm_to_nm(num_at(*d, 1));
            p.drill_slot_nm = mm_to_nm(num_at(*d, 2));
        } else {
            p.drill_nm = mm_to_nm(num_at(*d, 0));
        }
    }
    if (auto layers = node.find("layers")) {
        for (std::size_t i = 1; i < layers->size(); ++i)
            p.layers.push_back(as_str(layers->list()[i]));
    }
    if (auto r = node.find("roundrect_rratio")) p.roundrect_ratio = num_at(*r, 0);
    if (auto n = node.find("net")) {
        p.net = static_cast<int>(num_at(*n, 0));
        if (n->size() >= 3) p.net_name = as_str(n->list()[2]);
    }
    if (auto m = node.find("solder_mask_margin"))  p.solder_mask_margin_mm = num_at(*m, 0);
    if (auto m = node.find("solder_paste_margin")) p.solder_paste_margin_mm = num_at(*m, 0);
    p.uuid = parse_uuid(node);
    return p;
}

ItemPtr parse_footprint(const SExpr & node) {
    auto fp = std::make_shared<Footprint>();
    if (node.size() >= 2) fp->lib_id = as_str(node.list()[1]);
    if (auto l = node.find("layer")) fp->placement_layer = str_at(*l, 0);
    if (auto at = node.find("at")) {
        fp->at    = parse_at(*at);
        fp->angle = parse_at_angle(*at);
    }
    if (auto d = node.find("descr")) fp->descr = str_at(*d, 0);
    if (auto t = node.find("tags"))  fp->tags  = str_at(*t, 0);
    if (auto a = node.find("attr")) {
        for (std::size_t i = 1; i < a->size(); ++i) {
            if (i > 1) fp->attr += ' ';
            fp->attr += as_str(a->list()[i]);
        }
    }
    fp->fields = parse_property_list(node);
    for (const auto & c : node.list()) {
        if (!c->is_list()) continue;
        if      (c->head() == "pad") fp->pads.push_back(parse_pad(*c));
        else if (c->head() == "fp_line" || c->head() == "fp_arc" ||
                 c->head() == "fp_poly" || c->head() == "fp_circle" ||
                 c->head() == "fp_text" || c->head() == "fp_rect" ||
                 c->head() == "model")
            fp->raw_graphics_sexpr.push_back(sexpr::to_kicad_string(*c));
    }
    fp->uuid = parse_uuid(node);
    return fp;
}

ItemPtr parse_pcb_track(const SExpr & node) {
    auto t = std::make_shared<PcbTrack>();
    if (auto s = node.find("start")) t->start = { mm_to_nm(num_at(*s, 0)), mm_to_nm(num_at(*s, 1)) };
    if (auto e = node.find("end"))   t->end   = { mm_to_nm(num_at(*e, 0)), mm_to_nm(num_at(*e, 1)) };
    if (auto w = node.find("width")) t->width_nm = mm_to_nm(num_at(*w, 0));
    if (auto l = node.find("layer")) t->layer = str_at(*l, 0);
    if (auto n = node.find("net"))   t->net = static_cast<int>(num_at(*n, 0));
    if (node.find("locked"))         t->locked = true;
    t->uuid = parse_uuid(node);
    return t;
}
ItemPtr parse_pcb_arc(const SExpr & node) {
    auto a = std::make_shared<PcbArc>();
    if (auto s = node.find("start")) a->start = { mm_to_nm(num_at(*s, 0)), mm_to_nm(num_at(*s, 1)) };
    if (auto m = node.find("mid"))   a->mid   = { mm_to_nm(num_at(*m, 0)), mm_to_nm(num_at(*m, 1)) };
    if (auto e = node.find("end"))   a->end   = { mm_to_nm(num_at(*e, 0)), mm_to_nm(num_at(*e, 1)) };
    if (auto w = node.find("width")) a->width_nm = mm_to_nm(num_at(*w, 0));
    if (auto l = node.find("layer")) a->layer = str_at(*l, 0);
    if (auto n = node.find("net"))   a->net = static_cast<int>(num_at(*n, 0));
    a->uuid = parse_uuid(node);
    return a;
}
ItemPtr parse_pcb_via(const SExpr & node) {
    auto v = std::make_shared<PcbVia>();
    if (auto t = node.find("type")) v->via_type = str_at(*t, 0);
    if (auto at = node.find("at"))  v->at = parse_at(*at);
    if (auto s  = node.find("size"))  v->size_nm  = mm_to_nm(num_at(*s, 0));
    if (auto d  = node.find("drill")) v->drill_nm = mm_to_nm(num_at(*d, 0));
    if (auto l = node.find("layers")) {
        v->layers.clear();
        for (std::size_t i = 1; i < l->size(); ++i) v->layers.push_back(as_str(l->list()[i]));
    }
    if (auto n = node.find("net")) v->net = static_cast<int>(num_at(*n, 0));
    if (node.find("locked")) v->locked = true;
    if (node.find("free"))   v->free   = true;
    v->uuid = parse_uuid(node);
    return v;
}
ItemPtr parse_gr_line(const SExpr & node) {
    auto g = std::make_shared<GrLine>();
    if (auto s = node.find("start")) g->start = { mm_to_nm(num_at(*s, 0)), mm_to_nm(num_at(*s, 1)) };
    if (auto e = node.find("end"))   g->end   = { mm_to_nm(num_at(*e, 0)), mm_to_nm(num_at(*e, 1)) };
    if (auto st = node.find("stroke")) {
        if (auto w = st->find("width")) g->width_nm = mm_to_nm(num_at(*w, 0));
        if (auto t = st->find("type"))  g->stroke_type = str_at(*t, 0);
    }
    if (auto l = node.find("layer")) g->layer = str_at(*l, 0);
    g->uuid = parse_uuid(node);
    return g;
}
ItemPtr parse_gr_arc(const SExpr & node) {
    auto g = std::make_shared<GrArc>();
    if (auto s = node.find("start")) g->start = { mm_to_nm(num_at(*s, 0)), mm_to_nm(num_at(*s, 1)) };
    if (auto m = node.find("mid"))   g->mid   = { mm_to_nm(num_at(*m, 0)), mm_to_nm(num_at(*m, 1)) };
    if (auto e = node.find("end"))   g->end   = { mm_to_nm(num_at(*e, 0)), mm_to_nm(num_at(*e, 1)) };
    if (auto st = node.find("stroke")) if (auto w = st->find("width")) g->width_nm = mm_to_nm(num_at(*w, 0));
    if (auto l = node.find("layer")) g->layer = str_at(*l, 0);
    g->uuid = parse_uuid(node);
    return g;
}
ItemPtr parse_gr_circle(const SExpr & node) {
    auto g = std::make_shared<GrCircle>();
    if (auto c = node.find("center")) g->center = { mm_to_nm(num_at(*c, 0)), mm_to_nm(num_at(*c, 1)) };
    if (auto m = node.find("end"))    g->mid    = { mm_to_nm(num_at(*m, 0)), mm_to_nm(num_at(*m, 1)) };
    if (auto st = node.find("stroke")) if (auto w = st->find("width")) g->width_nm = mm_to_nm(num_at(*w, 0));
    if (auto l = node.find("layer")) g->layer = str_at(*l, 0);
    if (auto f = node.find("fill"))  g->fill_type = str_at(*f, 0);
    g->uuid = parse_uuid(node);
    return g;
}
ItemPtr parse_gr_polygon(const SExpr & node) {
    auto g = std::make_shared<GrPolygon>();
    if (auto pts = node.find("pts")) {
        for (auto & p : parse_pts(*pts)) g->outline.append(p);
        g->outline.set_closed(true);
    }
    if (auto st = node.find("stroke")) if (auto w = st->find("width")) g->width_nm = mm_to_nm(num_at(*w, 0));
    if (auto l = node.find("layer")) g->layer = str_at(*l, 0);
    if (auto f = node.find("fill"))  g->fill_type = str_at(*f, 0);
    g->uuid = parse_uuid(node);
    return g;
}
ItemPtr parse_gr_text(const SExpr & node) {
    auto g = std::make_shared<GrText>();
    if (node.size() >= 2) g->text = as_str(node.list()[1]);
    if (auto at = node.find("at")) {
        g->at    = parse_at(*at);
        g->angle = parse_at_angle(*at);
    }
    if (auto l = node.find("layer")) g->layer = str_at(*l, 0);
    if (auto eff = node.find("effects")) {
        Field tmp; parse_effects(*eff, tmp);
        g->font_h_mm = tmp.font_h_mm;
        g->font_v_mm = tmp.font_v_mm;
        g->bold      = tmp.bold;
        g->italic    = tmp.italic;
        g->justify   = tmp.justify;
    }
    g->uuid = parse_uuid(node);
    return g;
}

ItemPtr parse_zone(const SExpr & node) {
    auto z = std::make_shared<Zone>();
    if (auto n = node.find("net"))       z->net = static_cast<int>(num_at(*n, 0));
    if (auto nn = node.find("net_name")) z->net_name = str_at(*nn, 0);
    if (auto l = node.find("layers")) {
        z->layers.clear();
        for (std::size_t i = 1; i < l->size(); ++i) z->layers.push_back(as_str(l->list()[i]));
    } else if (auto l = node.find("layer")) {
        z->layers = { str_at(*l, 0) };
    }
    if (auto h = node.find("hatch")) {
        z->hatch_thickness_nm = mm_to_nm(num_at(*h, 0));
        z->hatch_gap_nm       = mm_to_nm(num_at(*h, 1));
    }
    if (auto f = node.find("fill")) {
        if (f->find("hatch_thickness")) z->fill_mode = "hatched";
    }
    if (auto c = node.find("connect_pads")) {
        if (auto clear = c->find("clearance")) z->clearance_nm = mm_to_nm(num_at(*clear, 0));
    }
    if (auto m = node.find("min_thickness")) z->min_thickness_nm = mm_to_nm(num_at(*m, 0));
    for (const auto & c : node.list()) {
        if (!c->is_list() || c->head() != "polygon") continue;
        if (auto pts = c->find("pts")) {
            geom::SHAPE_LINE_CHAIN chain;
            for (auto & p : parse_pts(*pts)) chain.append(p);
            chain.set_closed(true);
            z->polys.push_back(std::move(chain));
        }
    }
    z->uuid = parse_uuid(node);
    return z;
}

// -------------------- Board top-level --------------------

std::vector<LayerInfo> parse_layers(const SExpr & node) {
    std::vector<LayerInfo> out;
    for (const auto & c : node.list()) {
        if (!c->is_list()) continue;
        if (c->size() < 3) continue;
        LayerInfo L;
        L.id             = static_cast<int>(c->list()[0]->as_double());
        L.canonical_name = as_str(c->list()[1]);
        L.type           = as_str(c->list()[2]);
        L.user_name      = (c->size() >= 4 && c->list()[3]->is_string())
                              ? c->list()[3]->string()
                              : L.canonical_name;
        out.push_back(std::move(L));
    }
    return out;
}

std::vector<NetInfo> parse_nets(const SExpr & root) {
    std::vector<NetInfo> out;
    for (const auto & c : root.list()) {
        if (!c->is_list() || c->head() != "net") continue;
        NetInfo n;
        n.id   = static_cast<int>(num_at(*c, 0));
        n.name = str_at(*c, 1);
        out.push_back(n);
    }
    return out;
}

} // namespace

// =========================================================================
// Public: .kicad_sch reader
// =========================================================================

std::optional<Schematic> read_schematic(std::string_view text, IOError * err) {
    sexpr::ParseError pe;
    auto root = sexpr::parse(text, &pe);
    if (!root) {
        if (err) { err->message = pe.message; err->line = pe.line; err->column = pe.column; err->offset = pe.offset; }
        return std::nullopt;
    }
    if (root->head() != "kicad_sch") {
        if (err) err->message = "not a kicad_sch file";
        return std::nullopt;
    }
    Schematic sch;
    if (auto v = root->find("version"))          sch.version = str_at(*v, 0);
    if (auto g = root->find("generator"))        sch.generator = str_at(*g, 0);
    if (auto g = root->find("generator_version"))sch.generator_version = str_at(*g, 0);
    if (auto p = root->find("paper"))            sch.paper = str_at(*p, 0);
    sch.uuid = parse_uuid(*root);

    if (auto tb = root->find("title_block")) {
        if (auto t = tb->find("title")) sch.root.title = str_at(*t, 0);
        if (auto r = tb->find("rev"))   sch.root.rev   = str_at(*r, 0);
        for (int i = 1; i <= 9; ++i) {
            for (const auto & c : tb->list()) {
                if (!c->is_list() || c->head() != "comment") continue;
                int idx = static_cast<int>(num_at(*c, 0));
                if (idx == i && c->size() >= 3)
                    sch.root.comment[i-1] = as_str(c->list()[2]);
            }
        }
    }
    sch.root.paper = sch.paper;

    if (auto libs = root->find("lib_symbols")) {
        for (const auto & c : libs->list()) {
            if (!c->is_list() || c->head() != "symbol") continue;
            LibSymbol ls = parse_lib_symbol(*c);
            sch.lib_symbols[ls.lib_id] = std::move(ls);
        }
    }
    parse_sch_children(*root, sch.root);
    return sch;
}

std::optional<Schematic> read_schematic_file(std::string_view path, IOError * err) {
    std::string text = slurp(path);
    if (text.empty()) { if (err) err->message = "cannot read " + std::string(path); return std::nullopt; }
    return read_schematic(text, err);
}

// =========================================================================
// Public: .kicad_pcb reader
// =========================================================================

std::optional<Board> read_board(std::string_view text, IOError * err) {
    sexpr::ParseError pe;
    auto root = sexpr::parse(text, &pe);
    if (!root) {
        if (err) { err->message = pe.message; err->line = pe.line; err->column = pe.column; err->offset = pe.offset; }
        return std::nullopt;
    }
    if (root->head() != "kicad_pcb") {
        if (err) err->message = "not a kicad_pcb file";
        return std::nullopt;
    }
    Board b;
    if (auto v = root->find("version"))          b.version = str_at(*v, 0);
    if (auto g = root->find("generator"))        b.generator = str_at(*g, 0);
    if (auto g = root->find("generator_version"))b.generator_version = str_at(*g, 0);
    if (auto p = root->find("paper"))            b.paper = str_at(*p, 0);
    if (auto gen = root->find("general")) {
        if (auto t = gen->find("thickness")) b.thickness_mm = num_at(*t, 0, 1.6);
        if (auto lt = gen->find("legacy_teardrops")) b.legacy_teardrops = (str_at(*lt, 0) == "yes");
    }
    if (auto layers = root->find("layers")) b.layers = parse_layers(*layers);
    b.nets = parse_nets(*root);
    if (auto s = root->find("setup")) b.raw_setup_sexpr = sexpr::to_kicad_string(*s);
    for (const auto & c : root->list()) {
        if (!c->is_list()) continue;
        const std::string h = c->head();
        if      (h == "footprint")  b.items.push_back(parse_footprint(*c));
        else if (h == "segment")    b.items.push_back(parse_pcb_track(*c));
        else if (h == "arc")        b.items.push_back(parse_pcb_arc(*c));
        else if (h == "via")        b.items.push_back(parse_pcb_via(*c));
        else if (h == "zone")       b.items.push_back(parse_zone(*c));
        else if (h == "gr_line")    b.items.push_back(parse_gr_line(*c));
        else if (h == "gr_arc")     b.items.push_back(parse_gr_arc(*c));
        else if (h == "gr_circle")  b.items.push_back(parse_gr_circle(*c));
        else if (h == "gr_poly" || h == "gr_polygon")
                                    b.items.push_back(parse_gr_polygon(*c));
        else if (h == "gr_text")    b.items.push_back(parse_gr_text(*c));
    }
    // Tail (embedded_fonts, etc.) not otherwise consumed.
    for (const auto & c : root->list()) {
        if (!c->is_list()) continue;
        const std::string h = c->head();
        if (h == "embedded_fonts") b.raw_tail_sexpr.push_back(sexpr::to_kicad_string(*c));
    }
    return b;
}

std::optional<Board> read_board_file(std::string_view path, IOError * err) {
    std::string text = slurp(path);
    if (text.empty()) { if (err) err->message = "cannot read " + std::string(path); return std::nullopt; }
    return read_board(text, err);
}

// =========================================================================
// Writers
// =========================================================================

namespace {

// Emit "(at X Y R)" (R omitted when 0).
SExprPtr at_form(VECTOR2I v, EDA_ANGLE a) {
    auto l = sexpr::list("at");
    l->list().push_back(SExpr::make_number(format_mm(v.x)));
    l->list().push_back(SExpr::make_number(format_mm(v.y)));
    if (a.deg() != 0.0) l->list().push_back(SExpr::make_number(a.deg()));
    return l;
}

SExprPtr uuid_form(const UUID & u) {
    auto l = sexpr::list("uuid");
    l->list().push_back(SExpr::make_string(u.empty() ? kicad_model::make_uuid() : u));
    return l;
}

SExprPtr effects_form(const Field & f) {
    auto eff = sexpr::list("effects");
    auto font = sexpr::list("font");
    auto sz   = sexpr::list("size");
    sz->list().push_back(SExpr::make_number(f.font_h_mm));
    sz->list().push_back(SExpr::make_number(f.font_v_mm));
    font->list().push_back(sz);
    if (f.bold)   font->list().push_back(sexpr::list("bold"));
    if (f.italic) font->list().push_back(sexpr::list("italic"));
    eff->list().push_back(font);
    if (!f.justify.empty()) {
        auto j = sexpr::list("justify");
        j->list().push_back(SExpr::make_atom(f.justify));
        eff->list().push_back(j);
    }
    if (f.hide) {
        auto h = sexpr::list("hide");
        h->list().push_back(SExpr::make_atom("yes"));
        eff->list().push_back(h);
    }
    return eff;
}

SExprPtr property_form(const Field & f) {
    auto p = sexpr::list("property");
    p->list().push_back(SExpr::make_string(f.name));
    p->list().push_back(SExpr::make_string(f.value));
    p->list().push_back(at_form(f.at, f.angle));
    p->list().push_back(effects_form(f));
    return p;
}

SExprPtr pts_form(const std::vector<VECTOR2I> & pts) {
    auto p = sexpr::list("pts");
    for (auto & v : pts) {
        auto xy = sexpr::list("xy");
        xy->list().push_back(SExpr::make_number(format_mm(v.x)));
        xy->list().push_back(SExpr::make_number(format_mm(v.y)));
        p->list().push_back(xy);
    }
    return p;
}

SExprPtr stroke_form(double width_mm, const std::string & type) {
    auto s = sexpr::list("stroke");
    auto w = sexpr::list("width");
    w->list().push_back(SExpr::make_number(width_mm));
    s->list().push_back(w);
    auto t = sexpr::list("type");
    t->list().push_back(SExpr::make_atom(type.empty() ? "default" : type));
    s->list().push_back(t);
    return s;
}

// Item emitters ------------------------------------------------------

SExprPtr emit_sch_symbol(const SchSymbol & s) {
    auto n = sexpr::list("symbol");
    { auto id = sexpr::list("lib_id"); id->list().push_back(SExpr::make_string(s.lib_id)); n->list().push_back(id); }
    n->list().push_back(at_form(s.at, s.angle));
    { auto u = sexpr::list("unit"); u->list().push_back(SExpr::make_number(static_cast<long long>(s.unit))); n->list().push_back(u); }
    { auto x = sexpr::list("in_bom");   x->list().push_back(SExpr::make_atom(s.in_bom   ? "yes":"no")); n->list().push_back(x); }
    { auto x = sexpr::list("on_board"); x->list().push_back(SExpr::make_atom(s.on_board ? "yes":"no")); n->list().push_back(x); }
    { auto x = sexpr::list("dnp");      x->list().push_back(SExpr::make_atom(s.dnp      ? "yes":"no")); n->list().push_back(x); }
    n->list().push_back(uuid_form(s.uuid));
    for (const auto & f : s.fields) n->list().push_back(property_form(f));
    return n;
}

SExprPtr emit_sch_wire(const SchWire & w) {
    auto n = sexpr::list("wire");
    n->list().push_back(pts_form(w.pts));
    n->list().push_back(stroke_form(w.stroke_mm, w.stroke_type));
    n->list().push_back(uuid_form(w.uuid));
    return n;
}
SExprPtr emit_sch_bus(const SchBus & w) {
    auto n = sexpr::list("bus");
    n->list().push_back(pts_form(w.pts));
    n->list().push_back(stroke_form(w.stroke_mm, w.stroke_type));
    n->list().push_back(uuid_form(w.uuid));
    return n;
}
SExprPtr emit_junction(const SchJunction & j) {
    auto n = sexpr::list("junction");
    n->list().push_back(at_form(j.at, {}));
    { auto d = sexpr::list("diameter"); d->list().push_back(SExpr::make_number(j.diameter_mm)); n->list().push_back(d); }
    n->list().push_back(uuid_form(j.uuid));
    return n;
}
SExprPtr emit_no_connect(const SchNoConnect & nc) {
    auto n = sexpr::list("no_connect");
    n->list().push_back(at_form(nc.at, {}));
    n->list().push_back(uuid_form(nc.uuid));
    return n;
}
SExprPtr emit_label(const SchLabel & l, const std::string & head) {
    auto n = sexpr::list(head);
    n->list().push_back(SExpr::make_string(l.text));
    if (!l.shape.empty()) {
        auto sh = sexpr::list("shape");
        sh->list().push_back(SExpr::make_atom(l.shape));
        n->list().push_back(sh);
    }
    n->list().push_back(at_form(l.at, l.angle));
    for (const auto & f : l.fields) n->list().push_back(property_form(f));
    n->list().push_back(uuid_form(l.uuid));
    return n;
}
SExprPtr emit_text(const SchText & t) {
    auto n = sexpr::list("text");
    n->list().push_back(SExpr::make_string(t.text));
    n->list().push_back(at_form(t.at, t.angle));
    Field tmp; tmp.font_h_mm = t.font_h_mm; tmp.font_v_mm = t.font_v_mm; tmp.bold = t.bold; tmp.italic = t.italic; tmp.justify = t.justify;
    n->list().push_back(effects_form(tmp));
    n->list().push_back(uuid_form(t.uuid));
    return n;
}

SExprPtr emit_footprint(const Footprint & fp) {
    auto n = sexpr::list("footprint");
    n->list().push_back(SExpr::make_string(fp.lib_id));
    { auto l = sexpr::list("layer"); l->list().push_back(SExpr::make_string(fp.placement_layer)); n->list().push_back(l); }
    n->list().push_back(uuid_form(fp.uuid));
    n->list().push_back(at_form(fp.at, fp.angle));
    if (!fp.attr.empty()) {
        auto a = sexpr::list("attr");
        // attr may be space-separated tokens.
        std::size_t start = 0;
        while (start < fp.attr.size()) {
            auto sp = fp.attr.find(' ', start);
            std::string tok = fp.attr.substr(start, sp == std::string::npos ? std::string::npos : sp - start);
            if (!tok.empty()) a->list().push_back(SExpr::make_atom(tok));
            if (sp == std::string::npos) break;
            start = sp + 1;
        }
        n->list().push_back(a);
    }
    for (const auto & f : fp.fields) n->list().push_back(property_form(f));
    for (const auto & pad : fp.pads) {
        auto p = sexpr::list("pad");
        p->list().push_back(SExpr::make_string(pad.number));
        p->list().push_back(SExpr::make_atom(pad.kind));
        p->list().push_back(SExpr::make_atom(pad.shape));
        p->list().push_back(at_form(pad.at, pad.angle));
        { auto s = sexpr::list("size"); s->list().push_back(SExpr::make_number(nm_to_mm(pad.size.x))); s->list().push_back(SExpr::make_number(nm_to_mm(pad.size.y))); p->list().push_back(s); }
        if (pad.drill_nm > 0) {
            auto d = sexpr::list("drill");
            if (pad.drill_slot_nm > 0) {
                d->list().push_back(SExpr::make_atom("oval"));
                d->list().push_back(SExpr::make_number(nm_to_mm(pad.drill_nm)));
                d->list().push_back(SExpr::make_number(nm_to_mm(pad.drill_slot_nm)));
            } else {
                d->list().push_back(SExpr::make_number(nm_to_mm(pad.drill_nm)));
            }
            p->list().push_back(d);
        }
        {
            auto l = sexpr::list("layers");
            for (auto & ln : pad.layers) l->list().push_back(SExpr::make_string(ln));
            p->list().push_back(l);
        }
        if (pad.roundrect_ratio > 0) {
            auto r = sexpr::list("roundrect_rratio");
            r->list().push_back(SExpr::make_number(pad.roundrect_ratio));
            p->list().push_back(r);
        }
        if (pad.net > 0) {
            auto ne = sexpr::list("net");
            ne->list().push_back(SExpr::make_number(static_cast<long long>(pad.net)));
            ne->list().push_back(SExpr::make_string(pad.net_name));
            p->list().push_back(ne);
        }
        p->list().push_back(uuid_form(pad.uuid));
        n->list().push_back(p);
    }
    // Emit preserved raw graphics.
    for (const auto & rg : fp.raw_graphics_sexpr) {
        auto parsed = sexpr::parse(rg);
        if (parsed) n->list().push_back(parsed);
    }
    return n;
}

SExprPtr emit_pcb_track(const PcbTrack & t) {
    auto n = sexpr::list("segment");
    auto s = sexpr::list("start"); s->list().push_back(SExpr::make_number(nm_to_mm(t.start.x))); s->list().push_back(SExpr::make_number(nm_to_mm(t.start.y))); n->list().push_back(s);
    auto e = sexpr::list("end");   e->list().push_back(SExpr::make_number(nm_to_mm(t.end.x)));   e->list().push_back(SExpr::make_number(nm_to_mm(t.end.y)));   n->list().push_back(e);
    { auto w = sexpr::list("width"); w->list().push_back(SExpr::make_number(nm_to_mm(t.width_nm))); n->list().push_back(w); }
    { auto l = sexpr::list("layer"); l->list().push_back(SExpr::make_string(t.layer)); n->list().push_back(l); }
    { auto ne = sexpr::list("net"); ne->list().push_back(SExpr::make_number(static_cast<long long>(t.net))); n->list().push_back(ne); }
    n->list().push_back(uuid_form(t.uuid));
    return n;
}

SExprPtr emit_pcb_via(const PcbVia & v) {
    auto n = sexpr::list("via");
    { auto t = sexpr::list("type"); t->list().push_back(SExpr::make_atom(v.via_type)); n->list().push_back(t); }
    n->list().push_back(at_form(v.at, {}));
    { auto s = sexpr::list("size");  s->list().push_back(SExpr::make_number(nm_to_mm(v.size_nm)));  n->list().push_back(s); }
    { auto d = sexpr::list("drill"); d->list().push_back(SExpr::make_number(nm_to_mm(v.drill_nm))); n->list().push_back(d); }
    { auto l = sexpr::list("layers"); for (auto & ln : v.layers) l->list().push_back(SExpr::make_string(ln)); n->list().push_back(l); }
    { auto ne = sexpr::list("net"); ne->list().push_back(SExpr::make_number(static_cast<long long>(v.net))); n->list().push_back(ne); }
    n->list().push_back(uuid_form(v.uuid));
    return n;
}

SExprPtr emit_gr_line(const GrLine & g) {
    auto n = sexpr::list("gr_line");
    auto s = sexpr::list("start"); s->list().push_back(SExpr::make_number(nm_to_mm(g.start.x))); s->list().push_back(SExpr::make_number(nm_to_mm(g.start.y))); n->list().push_back(s);
    auto e = sexpr::list("end");   e->list().push_back(SExpr::make_number(nm_to_mm(g.end.x)));   e->list().push_back(SExpr::make_number(nm_to_mm(g.end.y)));   n->list().push_back(e);
    n->list().push_back(stroke_form(nm_to_mm(g.width_nm), g.stroke_type));
    { auto l = sexpr::list("layer"); l->list().push_back(SExpr::make_string(g.layer)); n->list().push_back(l); }
    n->list().push_back(uuid_form(g.uuid));
    return n;
}

// -------------------- Missing writer coverage --------------------

SExprPtr emit_bus_entry(const SchBusEntry & be) {
    auto n = sexpr::list("bus_entry");
    n->list().push_back(at_form(be.at, {}));
    { auto sz = sexpr::list("size"); sz->list().push_back(SExpr::make_number(nm_to_mm(be.size.x))); sz->list().push_back(SExpr::make_number(nm_to_mm(be.size.y))); n->list().push_back(sz); }
    n->list().push_back(stroke_form(be.stroke_mm, be.stroke_type));
    n->list().push_back(uuid_form(be.uuid));
    return n;
}

SExprPtr emit_sch_textbox(const SchTextBox & tb) {
    auto n = sexpr::list("text_box");
    n->list().push_back(SExpr::make_string(tb.text));
    n->list().push_back(at_form(tb.at, tb.angle));
    { auto sz = sexpr::list("size"); sz->list().push_back(SExpr::make_number(nm_to_mm(tb.size.x))); sz->list().push_back(SExpr::make_number(nm_to_mm(tb.size.y))); n->list().push_back(sz); }
    n->list().push_back(stroke_form(tb.stroke_mm, tb.stroke_type));
    { auto f = sexpr::list("fill"); auto t = sexpr::list("type"); t->list().push_back(SExpr::make_atom(tb.fill_type.empty() ? "none" : tb.fill_type)); f->list().push_back(t); n->list().push_back(f); }
    Field tmp; tmp.font_h_mm = tb.font_h_mm; tmp.font_v_mm = tb.font_v_mm;
    n->list().push_back(effects_form(tmp));
    n->list().push_back(uuid_form(tb.uuid));
    return n;
}

SExprPtr emit_sch_shape(const SchShape & s) {
    auto n = sexpr::list(s.shape.empty() ? std::string("polyline") : s.shape);
    if (s.shape == "polyline" || s.shape == "bezier") {
        auto p = sexpr::list("pts");
        for (const auto & pt : s.pts) {
            auto xy = sexpr::list("xy");
            xy->list().push_back(SExpr::make_number(nm_to_mm(pt.x)));
            xy->list().push_back(SExpr::make_number(nm_to_mm(pt.y)));
            p->list().push_back(xy);
        }
        n->list().push_back(p);
    } else if (s.shape == "rectangle") {
        { auto a = sexpr::list("start"); a->list().push_back(SExpr::make_number(nm_to_mm(s.start.x))); a->list().push_back(SExpr::make_number(nm_to_mm(s.start.y))); n->list().push_back(a); }
        { auto b = sexpr::list("end");   b->list().push_back(SExpr::make_number(nm_to_mm(s.end.x)));   b->list().push_back(SExpr::make_number(nm_to_mm(s.end.y)));   n->list().push_back(b); }
    } else if (s.shape == "circle") {
        { auto c = sexpr::list("center"); c->list().push_back(SExpr::make_number(nm_to_mm(s.center.x))); c->list().push_back(SExpr::make_number(nm_to_mm(s.center.y))); n->list().push_back(c); }
        { auto r = sexpr::list("radius"); r->list().push_back(SExpr::make_number(nm_to_mm(s.radius_nm))); n->list().push_back(r); }
    } else if (s.shape == "arc") {
        { auto a = sexpr::list("start"); a->list().push_back(SExpr::make_number(nm_to_mm(s.start.x))); a->list().push_back(SExpr::make_number(nm_to_mm(s.start.y))); n->list().push_back(a); }
        { auto m = sexpr::list("mid");   m->list().push_back(SExpr::make_number(nm_to_mm(s.mid.x)));   m->list().push_back(SExpr::make_number(nm_to_mm(s.mid.y)));   n->list().push_back(m); }
        { auto e = sexpr::list("end");   e->list().push_back(SExpr::make_number(nm_to_mm(s.end.x)));   e->list().push_back(SExpr::make_number(nm_to_mm(s.end.y)));   n->list().push_back(e); }
    }
    n->list().push_back(stroke_form(s.stroke_mm, s.stroke_type));
    { auto f = sexpr::list("fill"); auto t = sexpr::list("type"); t->list().push_back(SExpr::make_atom(s.fill_type.empty() ? "none" : s.fill_type)); f->list().push_back(t); n->list().push_back(f); }
    n->list().push_back(uuid_form(s.uuid));
    return n;
}

SExprPtr emit_sch_sheet(const SchSheet & sh) {
    auto n = sexpr::list("sheet");
    n->list().push_back(at_form(sh.at, {}));
    { auto sz = sexpr::list("size"); sz->list().push_back(SExpr::make_number(nm_to_mm(sh.size.x))); sz->list().push_back(SExpr::make_number(nm_to_mm(sh.size.y))); n->list().push_back(sz); }
    n->list().push_back(stroke_form(sh.stroke_mm, "default"));
    { auto f = sexpr::list("fill"); auto t = sexpr::list("type"); t->list().push_back(SExpr::make_atom(sh.fill_type.empty() ? "none" : sh.fill_type)); f->list().push_back(t); n->list().push_back(f); }
    n->list().push_back(uuid_form(sh.uuid));
    for (const auto & f : sh.fields) n->list().push_back(property_form(f));
    for (const auto & pin : sh.pins) {
        auto p = sexpr::list("pin");
        p->list().push_back(SExpr::make_string(pin.name));
        p->list().push_back(SExpr::make_atom(pin.shape.empty() ? "input" : pin.shape));
        p->list().push_back(at_form(pin.at, pin.angle));
        p->list().push_back(uuid_form(pin.uuid));
        n->list().push_back(p);
    }
    return n;
}

// -------- PCB missing coverage --------

SExprPtr emit_pcb_arc(const PcbArc & a) {
    auto n = sexpr::list("arc");
    { auto s = sexpr::list("start"); s->list().push_back(SExpr::make_number(nm_to_mm(a.start.x))); s->list().push_back(SExpr::make_number(nm_to_mm(a.start.y))); n->list().push_back(s); }
    { auto m = sexpr::list("mid");   m->list().push_back(SExpr::make_number(nm_to_mm(a.mid.x)));   m->list().push_back(SExpr::make_number(nm_to_mm(a.mid.y)));   n->list().push_back(m); }
    { auto e = sexpr::list("end");   e->list().push_back(SExpr::make_number(nm_to_mm(a.end.x)));   e->list().push_back(SExpr::make_number(nm_to_mm(a.end.y)));   n->list().push_back(e); }
    { auto w = sexpr::list("width"); w->list().push_back(SExpr::make_number(nm_to_mm(a.width_nm))); n->list().push_back(w); }
    { auto l = sexpr::list("layer"); l->list().push_back(SExpr::make_string(a.layer)); n->list().push_back(l); }
    { auto ne = sexpr::list("net");  ne->list().push_back(SExpr::make_number(static_cast<long long>(a.net))); n->list().push_back(ne); }
    n->list().push_back(uuid_form(a.uuid));
    return n;
}

SExprPtr emit_gr_arc(const GrArc & g) {
    auto n = sexpr::list("gr_arc");
    { auto s = sexpr::list("start"); s->list().push_back(SExpr::make_number(nm_to_mm(g.start.x))); s->list().push_back(SExpr::make_number(nm_to_mm(g.start.y))); n->list().push_back(s); }
    { auto m = sexpr::list("mid");   m->list().push_back(SExpr::make_number(nm_to_mm(g.mid.x)));   m->list().push_back(SExpr::make_number(nm_to_mm(g.mid.y)));   n->list().push_back(m); }
    { auto e = sexpr::list("end");   e->list().push_back(SExpr::make_number(nm_to_mm(g.end.x)));   e->list().push_back(SExpr::make_number(nm_to_mm(g.end.y)));   n->list().push_back(e); }
    n->list().push_back(stroke_form(nm_to_mm(g.width_nm), "default"));
    { auto l = sexpr::list("layer"); l->list().push_back(SExpr::make_string(g.layer)); n->list().push_back(l); }
    n->list().push_back(uuid_form(g.uuid));
    return n;
}

SExprPtr emit_gr_circle(const GrCircle & g) {
    auto n = sexpr::list("gr_circle");
    { auto c = sexpr::list("center"); c->list().push_back(SExpr::make_number(nm_to_mm(g.center.x))); c->list().push_back(SExpr::make_number(nm_to_mm(g.center.y))); n->list().push_back(c); }
    { auto m = sexpr::list("end");    m->list().push_back(SExpr::make_number(nm_to_mm(g.mid.x)));    m->list().push_back(SExpr::make_number(nm_to_mm(g.mid.y)));    n->list().push_back(m); }
    n->list().push_back(stroke_form(nm_to_mm(g.width_nm), "default"));
    { auto f = sexpr::list("fill"); auto t = sexpr::list("type"); t->list().push_back(SExpr::make_atom(g.fill_type.empty() ? "none" : g.fill_type)); f->list().push_back(t); n->list().push_back(f); }
    { auto l = sexpr::list("layer"); l->list().push_back(SExpr::make_string(g.layer)); n->list().push_back(l); }
    n->list().push_back(uuid_form(g.uuid));
    return n;
}

SExprPtr emit_gr_polygon(const GrPolygon & g) {
    auto n = sexpr::list("gr_poly");
    auto pts = sexpr::list("pts");
    for (std::size_t i = 0; i < g.outline.point_count(); ++i) {
        auto & p = g.outline.point(i);
        auto xy = sexpr::list("xy");
        xy->list().push_back(SExpr::make_number(nm_to_mm(p.x)));
        xy->list().push_back(SExpr::make_number(nm_to_mm(p.y)));
        pts->list().push_back(xy);
    }
    n->list().push_back(pts);
    n->list().push_back(stroke_form(nm_to_mm(g.width_nm), "default"));
    { auto f = sexpr::list("fill"); auto t = sexpr::list("type"); t->list().push_back(SExpr::make_atom(g.fill_type.empty() ? "none" : g.fill_type)); f->list().push_back(t); n->list().push_back(f); }
    { auto l = sexpr::list("layer"); l->list().push_back(SExpr::make_string(g.layer)); n->list().push_back(l); }
    n->list().push_back(uuid_form(g.uuid));
    return n;
}

SExprPtr emit_gr_text(const GrText & g) {
    auto n = sexpr::list("gr_text");
    n->list().push_back(SExpr::make_string(g.text));
    n->list().push_back(at_form(g.at, g.angle));
    { auto l = sexpr::list("layer"); l->list().push_back(SExpr::make_string(g.layer)); n->list().push_back(l); }
    Field tmp; tmp.font_h_mm = g.font_h_mm; tmp.font_v_mm = g.font_v_mm; tmp.bold = g.bold; tmp.italic = g.italic; tmp.justify = g.justify;
    n->list().push_back(effects_form(tmp));
    n->list().push_back(uuid_form(g.uuid));
    return n;
}

SExprPtr emit_zone(const Zone & z) {
    auto n = sexpr::list("zone");
    { auto ne = sexpr::list("net"); ne->list().push_back(SExpr::make_number(static_cast<long long>(z.net))); n->list().push_back(ne); }
    { auto nn = sexpr::list("net_name"); nn->list().push_back(SExpr::make_string(z.net_name)); n->list().push_back(nn); }
    { auto l = sexpr::list("layers"); for (auto & L : z.layers) l->list().push_back(SExpr::make_string(L)); n->list().push_back(l); }
    if (z.hatch_thickness_nm > 0) {
        auto h = sexpr::list("hatch");
        h->list().push_back(SExpr::make_number(nm_to_mm(z.hatch_thickness_nm)));
        h->list().push_back(SExpr::make_number(nm_to_mm(z.hatch_gap_nm)));
        n->list().push_back(h);
    }
    if (z.min_thickness_nm > 0) {
        auto m = sexpr::list("min_thickness");
        m->list().push_back(SExpr::make_number(nm_to_mm(z.min_thickness_nm)));
        n->list().push_back(m);
    }
    for (const auto & poly : z.polys) {
        auto p = sexpr::list("polygon");
        auto pts = sexpr::list("pts");
        for (std::size_t i = 0; i < poly.point_count(); ++i) {
            auto & pt = poly.point(i);
            auto xy = sexpr::list("xy");
            xy->list().push_back(SExpr::make_number(nm_to_mm(pt.x)));
            xy->list().push_back(SExpr::make_number(nm_to_mm(pt.y)));
            pts->list().push_back(xy);
        }
        p->list().push_back(pts);
        n->list().push_back(p);
    }
    n->list().push_back(uuid_form(z.uuid));
    return n;
}

} // namespace

std::string write_schematic(const Schematic & sch) {
    auto root = sexpr::list("kicad_sch");
    { auto n = sexpr::list("version"); n->list().push_back(SExpr::make_number(sch.version.empty() ? std::string("20250114") : sch.version)); root->list().push_back(n); }
    { auto n = sexpr::list("generator"); n->list().push_back(SExpr::make_string(sch.generator)); root->list().push_back(n); }
    { auto n = sexpr::list("generator_version"); n->list().push_back(SExpr::make_string(sch.generator_version)); root->list().push_back(n); }
    root->list().push_back(uuid_form(sch.uuid));
    { auto n = sexpr::list("paper"); n->list().push_back(SExpr::make_string(sch.paper)); root->list().push_back(n); }

    auto libs = sexpr::list("lib_symbols");
    for (const auto & kv : sch.lib_symbols) {
        auto s = sexpr::list("symbol");
        s->list().push_back(SExpr::make_string(kv.second.lib_id));
        // Fields
        for (const auto & f : kv.second.fields) s->list().push_back(property_form(f));
        // Pins
        for (const auto & p : kv.second.pins) {
            auto pn = sexpr::list("pin");
            pn->list().push_back(SExpr::make_atom(p.electrical.empty() ? "passive" : p.electrical));
            pn->list().push_back(SExpr::make_atom(p.shape.empty() ? "line" : p.shape));
            pn->list().push_back(at_form(p.at, p.angle));
            { auto l = sexpr::list("length"); l->list().push_back(SExpr::make_number(p.length_mm)); pn->list().push_back(l); }
            { auto n = sexpr::list("name"); n->list().push_back(SExpr::make_string(p.name.empty()? "~" : p.name)); pn->list().push_back(n); }
            { auto n = sexpr::list("number"); n->list().push_back(SExpr::make_string(p.number)); pn->list().push_back(n); }
            pn->list().push_back(uuid_form(p.uuid));
            s->list().push_back(pn);
        }
        // Preserved raw
        for (const auto & g : kv.second.raw_graphics_sexpr) { auto p = sexpr::parse(g); if (p) s->list().push_back(p); }
        for (const auto & u : kv.second.raw_unit_sexpr)     { auto p = sexpr::parse(u); if (p) s->list().push_back(p); }
        libs->list().push_back(s);
    }
    root->list().push_back(libs);

    for (const auto & it : sch.root.items) {
        switch (it->type) {
            case kicad_model::ItemType::SchSymbol:      root->list().push_back(emit_sch_symbol(*static_cast<const SchSymbol*>(it.get()))); break;
            case kicad_model::ItemType::SchWire:        root->list().push_back(emit_sch_wire  (*static_cast<const SchWire*>(it.get())));   break;
            case kicad_model::ItemType::SchBus:         root->list().push_back(emit_sch_bus   (*static_cast<const SchBus*>(it.get())));    break;
            case kicad_model::ItemType::SchJunction:    root->list().push_back(emit_junction  (*static_cast<const SchJunction*>(it.get()))); break;
            case kicad_model::ItemType::SchNoConnect:   root->list().push_back(emit_no_connect(*static_cast<const SchNoConnect*>(it.get()))); break;
            case kicad_model::ItemType::SchLabel:       root->list().push_back(emit_label(*static_cast<const SchLabel*>(it.get()),       "label"));       break;
            case kicad_model::ItemType::SchGlobalLabel: root->list().push_back(emit_label(*static_cast<const SchGlobalLabel*>(it.get()), "global_label")); break;
            case kicad_model::ItemType::SchHierLabel:   root->list().push_back(emit_label(*static_cast<const SchHierLabel*>(it.get()),   "hierarchical_label")); break;
            case kicad_model::ItemType::SchText:        root->list().push_back(emit_text(*static_cast<const SchText*>(it.get())));       break;
            case kicad_model::ItemType::SchTextBox:     root->list().push_back(emit_sch_textbox(*static_cast<const SchTextBox*>(it.get()))); break;
            case kicad_model::ItemType::SchShape:       root->list().push_back(emit_sch_shape(*static_cast<const SchShape*>(it.get())));   break;
            case kicad_model::ItemType::SchSheet:       root->list().push_back(emit_sch_sheet(*static_cast<const SchSheet*>(it.get())));   break;
            case kicad_model::ItemType::SchBusEntry:    root->list().push_back(emit_bus_entry(*static_cast<const SchBusEntry*>(it.get()))); break;
            default: break;
        }
    }

    { auto si = sexpr::list("sheet_instances"); auto pth = sexpr::list("path"); pth->list().push_back(SExpr::make_string("/")); auto pg = sexpr::list("page"); pg->list().push_back(SExpr::make_string("1")); pth->list().push_back(pg); si->list().push_back(pth); root->list().push_back(si); }
    { auto ef = sexpr::list("embedded_fonts"); ef->list().push_back(SExpr::make_atom("no")); root->list().push_back(ef); }

    return sexpr::to_kicad_string(*root);
}

bool write_schematic_file(std::string_view path, const Schematic & sch) {
    std::ofstream f{std::string(path), std::ios::binary};
    if (!f) return false;
    std::string s = write_schematic(sch);
    f.write(s.data(), static_cast<std::streamsize>(s.size()));
    return static_cast<bool>(f);
}

std::string write_board(const Board & b) {
    auto root = sexpr::list("kicad_pcb");
    { auto n = sexpr::list("version"); n->list().push_back(SExpr::make_number(b.version.empty() ? std::string("20241229") : b.version)); root->list().push_back(n); }
    { auto n = sexpr::list("generator"); n->list().push_back(SExpr::make_string(b.generator)); root->list().push_back(n); }
    { auto n = sexpr::list("generator_version"); n->list().push_back(SExpr::make_string(b.generator_version)); root->list().push_back(n); }
    { auto g = sexpr::list("general"); auto t = sexpr::list("thickness"); t->list().push_back(SExpr::make_number(b.thickness_mm)); g->list().push_back(t); auto lt = sexpr::list("legacy_teardrops"); lt->list().push_back(SExpr::make_atom(b.legacy_teardrops ? "yes" : "no")); g->list().push_back(lt); root->list().push_back(g); }
    { auto n = sexpr::list("paper"); n->list().push_back(SExpr::make_string(b.paper)); root->list().push_back(n); }
    auto layers = sexpr::list("layers");
    for (const auto & L : b.layers) {
        auto lst = sexpr::list("");
        lst->list().clear();
        lst->list().push_back(SExpr::make_number(static_cast<long long>(L.id)));
        lst->list().push_back(SExpr::make_string(L.canonical_name));
        lst->list().push_back(SExpr::make_atom(L.type));
        if (!L.user_name.empty() && L.user_name != L.canonical_name)
            lst->list().push_back(SExpr::make_string(L.user_name));
        layers->list().push_back(lst);
    }
    root->list().push_back(layers);

    if (!b.raw_setup_sexpr.empty()) {
        auto s = sexpr::parse(b.raw_setup_sexpr);
        if (s) root->list().push_back(s);
    }
    for (const auto & n : b.nets) {
        auto ne = sexpr::list("net");
        ne->list().push_back(SExpr::make_number(static_cast<long long>(n.id)));
        ne->list().push_back(SExpr::make_string(n.name));
        root->list().push_back(ne);
    }

    for (const auto & it : b.items) {
        switch (it->type) {
            case kicad_model::ItemType::PcbFootprint: root->list().push_back(emit_footprint(*static_cast<const Footprint*>(it.get()))); break;
            case kicad_model::ItemType::PcbTrack:     root->list().push_back(emit_pcb_track(*static_cast<const PcbTrack*>(it.get()))); break;
            case kicad_model::ItemType::PcbVia:       root->list().push_back(emit_pcb_via(*static_cast<const PcbVia*>(it.get()))); break;
            case kicad_model::ItemType::PcbGrLine:    root->list().push_back(emit_gr_line(*static_cast<const GrLine*>(it.get()))); break;
            case kicad_model::ItemType::PcbArc:       root->list().push_back(emit_pcb_arc(*static_cast<const PcbArc*>(it.get()))); break;
            case kicad_model::ItemType::PcbZone:      root->list().push_back(emit_zone(*static_cast<const Zone*>(it.get()))); break;
            case kicad_model::ItemType::PcbGrArc:     root->list().push_back(emit_gr_arc(*static_cast<const GrArc*>(it.get()))); break;
            case kicad_model::ItemType::PcbGrCircle:  root->list().push_back(emit_gr_circle(*static_cast<const GrCircle*>(it.get()))); break;
            case kicad_model::ItemType::PcbGrPolygon: root->list().push_back(emit_gr_polygon(*static_cast<const GrPolygon*>(it.get()))); break;
            case kicad_model::ItemType::PcbGrText:    root->list().push_back(emit_gr_text(*static_cast<const GrText*>(it.get()))); break;
            default: break;
        }
    }
    for (const auto & tail : b.raw_tail_sexpr) {
        auto p = sexpr::parse(tail);
        if (p) root->list().push_back(p);
    }
    return sexpr::to_kicad_string(*root);
}

bool write_board_file(std::string_view path, const Board & b) {
    std::ofstream f{std::string(path), std::ios::binary};
    if (!f) return false;
    std::string s = write_board(b);
    f.write(s.data(), static_cast<std::streamsize>(s.size()));
    return static_cast<bool>(f);
}

// =========================================================================
// Symbol library + footprint file: stubs delegating to model builders.
// Full round-trip coverage is a follow-up task; the shape is the same as
// the sch/pcb ones above.
// =========================================================================

std::optional<std::unordered_map<std::string, LibSymbol>>
    read_symbol_library(std::string_view text, IOError * err) {
    std::unordered_map<std::string, LibSymbol> out;
    sexpr::ParseError pe;
    auto root = sexpr::parse(text, &pe);
    if (!root) { if (err) err->message = pe.message; return std::nullopt; }
    if (root->head() != "kicad_symbol_lib") {
        // Some files omit the wrapper; treat as a bag of top-level symbols.
        if (root->head() == "symbol") {
            auto s = parse_lib_symbol(*root);
            out[s.lib_id] = std::move(s);
            return out;
        }
        if (err) err->message = "not a kicad_symbol_lib";
        return std::nullopt;
    }
    for (const auto & c : root->list()) {
        if (!c->is_list() || c->head() != "symbol") continue;
        auto s = parse_lib_symbol(*c);
        out[s.lib_id] = std::move(s);
    }
    return out;
}

std::string write_symbol_library(
    const std::unordered_map<std::string, LibSymbol> & lib,
    const std::string & /*lib_name*/) {
    auto root = sexpr::list("kicad_symbol_lib");
    { auto v = sexpr::list("version"); v->list().push_back(SExpr::make_number(std::string("20241209"))); root->list().push_back(v); }
    { auto g = sexpr::list("generator"); g->list().push_back(SExpr::make_string("ac9")); root->list().push_back(g); }
    for (const auto & kv : lib) {
        auto s = sexpr::list("symbol");
        s->list().push_back(SExpr::make_string(kv.second.lib_id));
        for (const auto & f : kv.second.fields) s->list().push_back(property_form(f));
        for (const auto & p : kv.second.pins) {
            auto pn = sexpr::list("pin");
            pn->list().push_back(SExpr::make_atom(p.electrical.empty() ? "passive" : p.electrical));
            pn->list().push_back(SExpr::make_atom(p.shape.empty()      ? "line"    : p.shape));
            pn->list().push_back(at_form(p.at, p.angle));
            { auto l = sexpr::list("length"); l->list().push_back(SExpr::make_number(p.length_mm)); pn->list().push_back(l); }
            { auto n = sexpr::list("name");   n->list().push_back(SExpr::make_string(p.name.empty() ? "~" : p.name)); pn->list().push_back(n); }
            { auto n = sexpr::list("number"); n->list().push_back(SExpr::make_string(p.number)); pn->list().push_back(n); }
            pn->list().push_back(uuid_form(p.uuid));
            s->list().push_back(pn);
        }
        for (const auto & g : kv.second.raw_graphics_sexpr) { auto p = sexpr::parse(g); if (p) s->list().push_back(p); }
        for (const auto & u : kv.second.raw_unit_sexpr)     { auto p = sexpr::parse(u); if (p) s->list().push_back(p); }
        root->list().push_back(s);
    }
    return sexpr::to_kicad_string(*root);
}

std::optional<Footprint> read_footprint(std::string_view text, IOError * err) {
    sexpr::ParseError pe;
    auto root = sexpr::parse(text, &pe);
    if (!root || root->head() != "footprint") {
        if (err) err->message = pe.message.empty() ? std::string("not a footprint") : pe.message;
        return std::nullopt;
    }
    auto fp = parse_footprint(*root);
    return *static_cast<Footprint*>(fp.get());
}

std::string write_footprint(const Footprint & fp) {
    return sexpr::to_kicad_string(*emit_footprint(fp));
}

} // namespace kicad_io
