// SPDX-License-Identifier: GPL-3.0-or-later
#include "sexpr.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace sexpr {

// ================== SExpr =============================================

SExprPtr SExpr::make_list() {
    auto e = std::make_shared<SExpr>();
    e->m_kind = Kind::List;
    return e;
}
SExprPtr SExpr::make_list(SExprList children) {
    auto e = std::make_shared<SExpr>();
    e->m_kind = Kind::List;
    e->m_children = std::move(children);
    return e;
}
SExprPtr SExpr::make_atom(std::string s) {
    auto e = std::make_shared<SExpr>();
    e->m_kind = Kind::Atom;
    e->m_val  = std::move(s);
    return e;
}
SExprPtr SExpr::make_string(std::string s) {
    auto e = std::make_shared<SExpr>();
    e->m_kind = Kind::String;
    e->m_val  = std::move(s);
    return e;
}
SExprPtr SExpr::make_number(std::string s) {
    auto e = std::make_shared<SExpr>();
    e->m_kind = Kind::Number;
    e->m_val  = std::move(s);
    return e;
}
SExprPtr SExpr::make_number(double d) {
    return make_number(format_double(d));
}
SExprPtr SExpr::make_number(long long i) {
    return make_number(std::to_string(i));
}
SExprPtr SExpr::make_comment(std::string s) {
    auto e = std::make_shared<SExpr>();
    e->m_kind = Kind::LineComment;
    e->m_val  = std::move(s);
    return e;
}

const SExprList &  SExpr::list()    const {
    if (m_kind != Kind::List)   throw std::runtime_error("SExpr::list() on non-list");
    return m_children;
}
SExprList & SExpr::list() {
    if (m_kind != Kind::List)   throw std::runtime_error("SExpr::list() on non-list");
    return m_children;
}
const std::string & SExpr::atom()   const {
    if (m_kind != Kind::Atom)   throw std::runtime_error("SExpr::atom() on non-atom");
    return m_val;
}
const std::string & SExpr::string() const {
    if (m_kind != Kind::String) throw std::runtime_error("SExpr::string() on non-string");
    return m_val;
}
const std::string & SExpr::number() const {
    if (m_kind != Kind::Number) throw std::runtime_error("SExpr::number() on non-number");
    return m_val;
}
const std::string & SExpr::comment() const {
    if (m_kind != Kind::LineComment) throw std::runtime_error("SExpr::comment() on non-comment");
    return m_val;
}

double SExpr::as_double() const {
    const auto & s = number();
    return std::strtod(s.c_str(), nullptr);
}
long long SExpr::as_int() const {
    const auto & s = number();
    return std::strtoll(s.c_str(), nullptr, 10);
}

std::size_t SExpr::size() const {
    if (m_kind != Kind::List) return 0;
    return m_children.size();
}
const SExpr & SExpr::operator[](std::size_t i) const {
    if (m_kind != Kind::List) throw std::runtime_error("SExpr::operator[] on non-list");
    return *m_children.at(i);
}

std::string SExpr::head() const {
    if (m_kind != Kind::List || m_children.empty()) return {};
    const auto & h = m_children.front();
    if (h->kind() == Kind::Atom)   return h->atom();
    if (h->kind() == Kind::String) return h->string();
    return {};
}

SExprPtr SExpr::find(std::string_view name) const {
    if (m_kind != Kind::List) return nullptr;
    for (const auto & c : m_children) {
        if (c->kind() != Kind::List) continue;
        if (c->head() == name) return c;
    }
    return nullptr;
}

std::vector<SExprPtr> SExpr::find_all(std::string_view name) const {
    std::vector<SExprPtr> out;
    if (m_kind != Kind::List) return out;
    for (const auto & c : m_children) {
        if (c->kind() != Kind::List) continue;
        if (c->head() == name) out.push_back(c);
    }
    return out;
}

SExprPtr SExpr::child_after_head(std::size_t i) const {
    if (m_kind != Kind::List) return nullptr;
    // Head is at index 0. child_after_head(0) = index 1, etc.
    std::size_t idx = i + 1;
    if (idx >= m_children.size()) return nullptr;
    return m_children[idx];
}

// ================== Parser ============================================

namespace {

struct Cursor {
    const char * data;
    std::size_t  size;
    std::size_t  pos    = 0;
    std::size_t  line   = 1;
    std::size_t  column = 1;

    bool eof() const { return pos >= size; }
    char peek() const { return eof() ? '\0' : data[pos]; }
    char get() {
        if (eof()) return '\0';
        char c = data[pos++];
        if (c == '\n') { ++line; column = 1; }
        else           { ++column; }
        return c;
    }
};

void skip_ws_and_capture_comments(Cursor & cur, SExprList * sink) {
    for (;;) {
        // Consume whitespace.
        while (!cur.eof()) {
            char c = cur.peek();
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { cur.get(); continue; }
            break;
        }
        if (cur.eof()) return;
        char c = cur.peek();
        // KiCad occasionally has "#" or ";" comments (not in native
        // schematic/pcb files, but in fp-lib-table / sym-lib-table
        // sometimes). Preserve them so round-trip is exact.
        if (c == '#' || c == ';') {
            std::string com;
            while (!cur.eof() && cur.peek() != '\n') com += cur.get();
            if (sink) sink->push_back(SExpr::make_comment(com));
            continue;
        }
        return;
    }
}

bool is_atom_start(char c) {
    if (c == '(' || c == ')' || c == '"' || c == ';' || c == '#') return false;
    return !(c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\0');
}

bool looks_like_number(std::string_view s) {
    if (s.empty()) return false;
    std::size_t i = 0;
    if (s[i] == '+' || s[i] == '-') ++i;
    bool any_digit = false;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) { ++i; any_digit = true; }
    if (i < s.size() && s[i] == '.') {
        ++i;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) { ++i; any_digit = true; }
    }
    if (!any_digit) return false;
    if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
        ++i;
        if (i < s.size() && (s[i] == '+' || s[i] == '-')) ++i;
        bool exp_digit = false;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) { ++i; exp_digit = true; }
        if (!exp_digit) return false;
    }
    return i == s.size();
}

SExprPtr parse_string(Cursor & cur, ParseError * err) {
    // Assumes current char is '"'.
    cur.get(); // consume opening quote
    std::string raw;
    while (!cur.eof()) {
        char c = cur.get();
        if (c == '\\') {
            if (cur.eof()) break;
            char n = cur.get();
            switch (n) {
                case 'n':  raw += '\n'; break;
                case 't':  raw += '\t'; break;
                case 'r':  raw += '\r'; break;
                case '"':  raw += '"';  break;
                case '\\': raw += '\\'; break;
                default:   raw += n;    break;
            }
            continue;
        }
        if (c == '"') return SExpr::make_string(std::move(raw));
        raw += c;
    }
    if (err) { err->message = "unterminated string"; err->line = cur.line; err->column = cur.column; err->offset = cur.pos; }
    return nullptr;
}

SExprPtr parse_atom_or_number(Cursor & cur) {
    std::string tok;
    while (!cur.eof()) {
        char c = cur.peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n' ||
            c == '(' || c == ')' || c == '"') break;
        tok += cur.get();
    }
    if (looks_like_number(tok)) return SExpr::make_number(std::move(tok));
    return SExpr::make_atom(std::move(tok));
}

SExprPtr parse_form(Cursor & cur, ParseError * err) {
    skip_ws_and_capture_comments(cur, nullptr);
    if (cur.eof()) return nullptr;
    char c = cur.peek();
    if (c == '(') {
        cur.get();
        auto lst = SExpr::make_list();
        for (;;) {
            skip_ws_and_capture_comments(cur, &lst->list());
            if (cur.eof()) {
                if (err) { err->message = "unterminated list"; err->line = cur.line; err->column = cur.column; err->offset = cur.pos; }
                return nullptr;
            }
            if (cur.peek() == ')') { cur.get(); return lst; }
            auto child = parse_form(cur, err);
            if (!child) return nullptr;
            lst->list().push_back(std::move(child));
        }
    }
    if (c == '"') return parse_string(cur, err);
    if (c == ')') {
        if (err) { err->message = "unexpected ')'"; err->line = cur.line; err->column = cur.column; err->offset = cur.pos; }
        return nullptr;
    }
    if (!is_atom_start(c)) {
        if (err) { err->message = "unexpected character"; err->line = cur.line; err->column = cur.column; err->offset = cur.pos; }
        return nullptr;
    }
    return parse_atom_or_number(cur);
}

} // namespace

SExprPtr parse(std::string_view text, ParseError * err) {
    Cursor cur{ text.data(), text.size() };
    auto root = parse_form(cur, err);
    if (!root) return nullptr;
    skip_ws_and_capture_comments(cur, nullptr);
    if (!cur.eof()) {
        if (err) { err->message = "trailing input after root form"; err->line = cur.line; err->column = cur.column; err->offset = cur.pos; }
        // Still return the root; some callers may want it.
    }
    return root;
}

std::vector<SExprPtr> parse_all(std::string_view text, ParseError * err) {
    std::vector<SExprPtr> out;
    Cursor cur{ text.data(), text.size() };
    for (;;) {
        skip_ws_and_capture_comments(cur, nullptr);
        if (cur.eof()) break;
        auto n = parse_form(cur, err);
        if (!n) break;
        out.push_back(std::move(n));
    }
    return out;
}

// ================== Writer ============================================

std::string escape_string(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 4);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\t': out += "\\t";  break;
            case '\r': out += "\\r";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

std::string unescape_string(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char n = s[++i];
            switch (n) {
                case 'n':  out += '\n'; break;
                case 't':  out += '\t'; break;
                case 'r':  out += '\r'; break;
                case '"':  out += '"';  break;
                case '\\': out += '\\'; break;
                default:   out += n;    break;
            }
        } else out += s[i];
    }
    return out;
}

std::string format_double(double d) {
    if (!std::isfinite(d)) return "0";
    // KiCad's convention: use "%g" style then normalize.
    char buf[64];
    // Six significant digits is what KiCad uses in most emitters.
    std::snprintf(buf, sizeof(buf), "%.6f", d);
    std::string s = buf;
    // Trim trailing zeros after decimal.
    if (s.find('.') != std::string::npos) {
        while (s.size() > 1 && s.back() == '0') s.pop_back();
        if (!s.empty() && s.back() == '.') s.pop_back();
    }
    if (s.empty() || s == "-0") s = "0";
    return s;
}

namespace {

bool all_leaves(const SExpr & n) {
    if (!n.is_list()) return false;
    for (const auto & c : n.list()) {
        if (c->is_list())    return false;
        if (c->is_comment()) return false;
    }
    return true;
}

std::string indent_str(int depth, const WriteOptions & opts) {
    if (opts.use_tabs) return std::string(static_cast<std::size_t>(depth), '\t');
    return std::string(static_cast<std::size_t>(depth * opts.indent_width), ' ');
}

void write_atom(std::string & out, const SExpr & n) {
    switch (n.kind()) {
        case Kind::Atom:   out += n.atom();                                       break;
        case Kind::Number: out += n.number();                                     break;
        case Kind::String: out += '"'; out += escape_string(n.string()); out += '"'; break;
        case Kind::LineComment: out += n.comment();                               break;
        case Kind::List:   /* handled elsewhere */                                break;
    }
}

void write_list(std::string & out, const SExpr & n, int depth, const WriteOptions & opts) {
    // Inline short leaf-only lists: (at 12 25 90), (fill (type ...)) etc.
    if (opts.inline_short && all_leaves(n) && n.size() <= opts.inline_max) {
        out += '(';
        for (std::size_t i = 0; i < n.size(); ++i) {
            if (i) out += ' ';
            write_atom(out, n[i]);
        }
        out += ')';
        return;
    }
    out += '(';
    // Head + subsequent children on new lines, indented.
    if (n.size() > 0) {
        // Emit head inline with the '('.
        write_atom(out, n[0]);
        for (std::size_t i = 1; i < n.size(); ++i) {
            const auto & c = n[i];
            out += '\n';
            out += indent_str(depth + 1, opts);
            // operator[] returns const SExpr & now (not SExprPtr), so
            // c is a reference, not a pointer -- use dot syntax.
            if (c.is_list()) write_list(out, c, depth + 1, opts);
            else             write_atom(out, c);
        }
        out += '\n';
        out += indent_str(depth, opts);
    }
    out += ')';
}

} // namespace

std::string to_kicad_string(const SExpr & root, const WriteOptions & opts) {
    std::string out;
    if (root.is_list()) write_list(out, root, 0, opts);
    else                write_atom(out, root);
    if (opts.trailing_newline) out += '\n';
    return out;
}

// --- Builders -------------------------------------------------------

SExprPtr list(std::string head) {
    auto l = SExpr::make_list();
    l->list().push_back(SExpr::make_atom(std::move(head)));
    return l;
}

SExprPtr list(std::string head, std::initializer_list<SExprPtr> kids) {
    auto l = list(std::move(head));
    for (auto & k : kids) l->list().push_back(k);
    return l;
}

} // namespace sexpr
