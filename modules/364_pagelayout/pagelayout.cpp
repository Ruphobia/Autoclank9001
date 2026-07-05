// SPDX-License-Identifier: GPL-3.0-or-later
#include "pagelayout.hpp"

#include "../344_sexpr/sexpr.hpp"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

namespace pagelayout {

using geom::mm_to_nm;
using geom::nm_to_mm;

namespace {

double num(const sexpr::SExpr & n, std::size_t i, double dflt = 0.0) {
    auto c = n.child_after_head(i);
    return c && c->is_number() ? c->as_double() : dflt;
}
std::string str(const sexpr::SExpr & n, std::size_t i) {
    auto c = n.child_after_head(i);
    if (!c) return {};
    if (c->is_string()) return c->string();
    if (c->is_atom())   return c->atom();
    return {};
}

geom::VECTOR2I parse_pos(const sexpr::SExpr & node, std::string_view key) {
    auto p = node.find(key);
    if (!p) return {0, 0};
    return { mm_to_nm(num(*p, 0)), mm_to_nm(num(*p, 1)) };
}

} // namespace

DrawingSheet read(std::string_view text) {
    DrawingSheet ds;
    sexpr::ParseError err;
    auto root = sexpr::parse(text, &err);
    if (!root || root->head() != "kicad_wks") return ds;

    if (auto s = root->find("setup")) {
        if (auto n = s->find("name"))       ds.setup_name = str(*n, 0);
        if (auto p = s->find("page"))       ds.page_width_mm  = num(*p, 0, 297.0);
        if (auto p = s->find("page")) if (p->child_after_head(1)) ds.page_height_mm = num(*p, 1, 210.0);
        if (auto l = s->find("left_margin"))   ds.left_margin_mm   = num(*l, 0, 10.0);
        if (auto l = s->find("right_margin"))  ds.right_margin_mm  = num(*l, 0, 10.0);
        if (auto l = s->find("top_margin"))    ds.top_margin_mm    = num(*l, 0, 10.0);
        if (auto l = s->find("bottom_margin")) ds.bottom_margin_mm = num(*l, 0, 10.0);
    }
    for (const auto & c : root->list()) {
        if (!c->is_list()) continue;
        const std::string h = c->head();
        if (h == "line") {
            DsLine L;
            L.start = parse_pos(*c, "start");
            L.end   = parse_pos(*c, "end");
            if (auto w = c->find("linewidth")) L.width_mm = num(*w, 0, 0.15);
            ds.lines.push_back(L);
        } else if (h == "rect") {
            DsRect R;
            R.start = parse_pos(*c, "start");
            R.end   = parse_pos(*c, "end");
            if (auto w = c->find("linewidth")) R.width_mm = num(*w, 0, 0.15);
            ds.rects.push_back(R);
        } else if (h == "tbtext") {
            DsText T;
            if (c->size() >= 2) T.text = str(*c, 0);
            T.at = parse_pos(*c, "pos");
            if (auto n = c->find("name")) T.name = str(*n, 0);
            if (auto f = c->find("font")) {
                if (auto sz = f->find("size")) {
                    T.font_h_mm = num(*sz, 0, 1.5);
                    T.font_v_mm = num(*sz, 1, 1.5);
                }
                if (f->find("bold"))   T.bold = true;
                if (f->find("italic")) T.italic = true;
            }
            if (auto j = c->find("justify")) T.justify = str(*j, 0);
            ds.texts.push_back(T);
        }
    }
    return ds;
}

std::string write(const DrawingSheet & ds) {
    auto root = sexpr::list("kicad_wks");
    { auto v = sexpr::list("version"); v->list().push_back(sexpr::SExpr::make_number(std::string("20220228"))); root->list().push_back(v); }
    { auto g = sexpr::list("generator"); g->list().push_back(sexpr::SExpr::make_string("ac9")); root->list().push_back(g); }

    auto setup = sexpr::list("setup");
    { auto n = sexpr::list("textsize"); n->list().push_back(sexpr::SExpr::make_number(1.5)); n->list().push_back(sexpr::SExpr::make_number(1.5)); setup->list().push_back(n); }
    { auto n = sexpr::list("linewidth"); n->list().push_back(sexpr::SExpr::make_number(0.15)); setup->list().push_back(n); }
    { auto n = sexpr::list("textlinewidth"); n->list().push_back(sexpr::SExpr::make_number(0.15)); setup->list().push_back(n); }
    { auto n = sexpr::list("left_margin"); n->list().push_back(sexpr::SExpr::make_number(ds.left_margin_mm)); setup->list().push_back(n); }
    { auto n = sexpr::list("right_margin"); n->list().push_back(sexpr::SExpr::make_number(ds.right_margin_mm)); setup->list().push_back(n); }
    { auto n = sexpr::list("top_margin"); n->list().push_back(sexpr::SExpr::make_number(ds.top_margin_mm)); setup->list().push_back(n); }
    { auto n = sexpr::list("bottom_margin"); n->list().push_back(sexpr::SExpr::make_number(ds.bottom_margin_mm)); setup->list().push_back(n); }
    root->list().push_back(setup);

    for (const auto & L : ds.lines) {
        auto n = sexpr::list("line");
        auto s = sexpr::list("start"); s->list().push_back(sexpr::SExpr::make_number(nm_to_mm(L.start.x))); s->list().push_back(sexpr::SExpr::make_number(nm_to_mm(L.start.y))); n->list().push_back(s);
        auto e = sexpr::list("end");   e->list().push_back(sexpr::SExpr::make_number(nm_to_mm(L.end.x)));   e->list().push_back(sexpr::SExpr::make_number(nm_to_mm(L.end.y)));   n->list().push_back(e);
        auto w = sexpr::list("linewidth"); w->list().push_back(sexpr::SExpr::make_number(L.width_mm)); n->list().push_back(w);
        root->list().push_back(n);
    }
    for (const auto & R : ds.rects) {
        auto n = sexpr::list("rect");
        auto s = sexpr::list("start"); s->list().push_back(sexpr::SExpr::make_number(nm_to_mm(R.start.x))); s->list().push_back(sexpr::SExpr::make_number(nm_to_mm(R.start.y))); n->list().push_back(s);
        auto e = sexpr::list("end");   e->list().push_back(sexpr::SExpr::make_number(nm_to_mm(R.end.x)));   e->list().push_back(sexpr::SExpr::make_number(nm_to_mm(R.end.y)));   n->list().push_back(e);
        auto w = sexpr::list("linewidth"); w->list().push_back(sexpr::SExpr::make_number(R.width_mm)); n->list().push_back(w);
        root->list().push_back(n);
    }
    for (const auto & T : ds.texts) {
        auto n = sexpr::list("tbtext");
        n->list().push_back(sexpr::SExpr::make_string(T.text));
        if (!T.name.empty()) {
            auto nm = sexpr::list("name"); nm->list().push_back(sexpr::SExpr::make_string(T.name)); n->list().push_back(nm);
        }
        auto p = sexpr::list("pos"); p->list().push_back(sexpr::SExpr::make_number(nm_to_mm(T.at.x))); p->list().push_back(sexpr::SExpr::make_number(nm_to_mm(T.at.y))); n->list().push_back(p);
        auto font = sexpr::list("font");
        auto sz = sexpr::list("size"); sz->list().push_back(sexpr::SExpr::make_number(T.font_h_mm)); sz->list().push_back(sexpr::SExpr::make_number(T.font_v_mm)); font->list().push_back(sz);
        if (T.bold)   font->list().push_back(sexpr::list("bold"));
        if (T.italic) font->list().push_back(sexpr::list("italic"));
        n->list().push_back(font);
        if (!T.justify.empty()) {
            auto j = sexpr::list("justify"); j->list().push_back(sexpr::SExpr::make_atom(T.justify)); n->list().push_back(j);
        }
        root->list().push_back(n);
    }
    return sexpr::to_kicad_string(*root);
}

std::string expand(std::string_view raw,
                   const std::vector<std::pair<std::string, std::string>> & vars) {
    std::string out;
    for (std::size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '$' && i + 1 < raw.size() && raw[i + 1] == '{') {
            auto close = raw.find('}', i + 2);
            if (close != std::string_view::npos) {
                std::string name(raw.substr(i + 2, close - i - 2));
                std::string val;
                for (const auto & kv : vars) if (kv.first == name) { val = kv.second; break; }
                out += val;
                i = close;
                continue;
            }
        }
        out += raw[i];
    }
    return out;
}

DrawingSheet default_a4_titleblock() {
    DrawingSheet ds;
    ds.page_width_mm  = 297;
    ds.page_height_mm = 210;

    // Outer border rectangle just inside margins.
    ds.rects.push_back({ { mm_to_nm(ds.left_margin_mm), mm_to_nm(ds.top_margin_mm) },
                         { mm_to_nm(ds.page_width_mm - ds.right_margin_mm),
                           mm_to_nm(ds.page_height_mm - ds.bottom_margin_mm) },
                         0.15 });

    // Title block box (bottom-right corner, 90 mm wide, 30 mm tall).
    double bx1 = ds.page_width_mm - ds.right_margin_mm - 90;
    double by1 = ds.page_height_mm - ds.bottom_margin_mm - 30;
    double bx2 = ds.page_width_mm - ds.right_margin_mm;
    double by2 = ds.page_height_mm - ds.bottom_margin_mm;
    ds.rects.push_back({ {mm_to_nm(bx1), mm_to_nm(by1)}, {mm_to_nm(bx2), mm_to_nm(by2)}, 0.15 });

    // Variable-expanded texts.
    ds.texts.push_back({ "Title: ${TITLE}",  { mm_to_nm(bx1 + 2), mm_to_nm(by1 + 6)  }, {}, 2, 2, 0.15, true, false, "left",  "title"});
    ds.texts.push_back({ "Author: ${AUTHOR}",{ mm_to_nm(bx1 + 2), mm_to_nm(by1 + 14) }, {}, 1.5, 1.5, 0.15, false, false, "left",  "author"});
    ds.texts.push_back({ "Rev: ${REV}",      { mm_to_nm(bx1 + 2), mm_to_nm(by1 + 22) }, {}, 1.5, 1.5, 0.15, false, false, "left",  "rev"});
    ds.texts.push_back({ "Date: ${DATE}",    { mm_to_nm(bx2 - 2), mm_to_nm(by1 + 22) }, {}, 1.5, 1.5, 0.15, false, false, "right", "date"});
    return ds;
}

} // namespace pagelayout
