// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

// Full s-expression parser and writer for KiCad file formats.
//
// This is the bedrock module for every KiCad-in-tool file operation.
// Round-trip fidelity is the contract: for any well-formed .kicad_sch /
// .kicad_pcb / .kicad_sym / .kicad_mod / .kicad_pro, the sequence
//   parse -> to_kicad_string -> parse
// produces the same tree. Whitespace within tokens is preserved; the
// canonical writer uses tabs like KiCad does.
//
// Values:
//   SExpr::List           -> ( ... nested ... )
//   SExpr::Atom           -> bare identifier or keyword
//   SExpr::String         -> "double-quoted, escapes preserved"
//   SExpr::Number         -> "12.7" style; kept as string so we don't
//                            lose precision or trailing zeros KiCad
//                            uses to distinguish 0.0 from 0.
//   SExpr::LineComment    -> preserved so we don't drop KiCad's own
//                            (# comment) or shell-style comments.
//
// Parsing is strict about paren balance and unterminated strings; it
// accepts KiCad's variants (tabs, CRLF line endings) transparently.
namespace sexpr {

class SExpr;
using SExprPtr  = std::shared_ptr<SExpr>;
using SExprList = std::vector<SExprPtr>;

enum class Kind : std::uint8_t {
    List,
    Atom,        // bare word: identifier, keyword, unquoted symbol
    String,      // "quoted"
    Number,      // 12.7, -1e-6, 0, .5
    LineComment  // "# ..." or ";; ..."; preserved on read
};

class SExpr {
public:
    // Construction helpers.
    static SExprPtr make_list();
    static SExprPtr make_list(SExprList children);
    static SExprPtr make_atom(std::string s);
    static SExprPtr make_string(std::string s);   // s is the RAW value (no escaping)
    static SExprPtr make_number(std::string s);   // canonical decimal text
    static SExprPtr make_number(double d);        // formatted with KiCad-style precision
    static SExprPtr make_number(long long i);
    static SExprPtr make_comment(std::string s);  // full comment text incl. leading #/;

    Kind        kind()   const noexcept { return m_kind; }
    bool        is_list()   const noexcept { return m_kind == Kind::List; }
    bool        is_atom()   const noexcept { return m_kind == Kind::Atom; }
    bool        is_string() const noexcept { return m_kind == Kind::String; }
    bool        is_number() const noexcept { return m_kind == Kind::Number; }
    bool        is_comment()const noexcept { return m_kind == Kind::LineComment; }

    // Value accessors. Kind-safe: mismatched calls throw.
    const SExprList &  list()   const;
    SExprList &        list();
    const std::string & atom()   const;
    const std::string & string() const;
    const std::string & number() const;
    const std::string & comment()const;

    // Numeric helpers on Number nodes.
    double     as_double() const;
    long long  as_int()    const;

    // List convenience.
    std::size_t   size() const;
    const SExpr & operator[](std::size_t i) const;

    // The head of a list: `(symbol "name" ...)` -> "symbol".
    // Returns empty string if not a list or list has no head.
    std::string head() const;

    // First child that is a list whose head atom equals `name`.
    // For queries like `(footprint ...).find("layer")`.
    SExprPtr find(std::string_view name) const;

    // All children that are lists with the given head. Empty vector
    // when not a list or no matches.
    std::vector<SExprPtr> find_all(std::string_view name) const;

    // Extract a value from a `(key value ...)` form. Common in KiCad:
    //   (at 12.7 25.4 90) -> at().as_double_list() -> {12.7, 25.4, 90}
    // Returns the child at index `i` past the head. Nullptr if OOB.
    SExprPtr child_after_head(std::size_t i) const;

    SExpr()                        = default;
    SExpr(const SExpr &)           = default;
    SExpr & operator=(const SExpr &) = default;
    SExpr(SExpr &&) noexcept        = default;
    SExpr & operator=(SExpr &&) noexcept = default;

private:
    Kind        m_kind = Kind::Atom;
    std::string m_val;
    SExprList   m_children;
};

// --- Parser ---------------------------------------------------------

struct ParseError {
    std::string message;
    std::size_t line   = 0;
    std::size_t column = 0;
    std::size_t offset = 0;
};

// Parse a top-level document. A KiCad file is a single top-level list,
// so this returns exactly one root SExpr (a List). Comments and
// whitespace outside the root are preserved as sibling nodes and
// bundled into the root's leading whitespace section internally.
//
// On failure returns nullptr and populates `err`.
SExprPtr parse(std::string_view text, ParseError * err = nullptr);

// Parse and return every top-level form. Useful when the input is a
// sequence rather than a single root (some tool-authored fragments).
std::vector<SExprPtr> parse_all(std::string_view text, ParseError * err = nullptr);

// --- Writer ---------------------------------------------------------

struct WriteOptions {
    // KiCad emits tab-indented output. When false, uses spaces.
    bool use_tabs        = true;
    // Spaces per indent when use_tabs is false.
    int  indent_width    = 4;
    // KiCad pretty-prints most forms but inlines short leaves like
    // (at 12 25 90). When true, we inline any list whose children are
    // all atoms/numbers/strings and whose length is under `inline_max`.
    bool inline_short    = true;
    std::size_t inline_max = 6;
    // Emit a trailing newline.
    bool trailing_newline = true;
};

// Emit the tree back to KiCad-canonical text.
std::string to_kicad_string(const SExpr & root, const WriteOptions & opts = {});

// Escape a raw string for inclusion in an s-expression string literal.
// Handles: \ " and control chars per KiCad's rules.
std::string escape_string(std::string_view s);
// Reverse.
std::string unescape_string(std::string_view s);

// KiCad-style number formatting: `12.700000` collapses to `12.7`,
// `0.000000` collapses to `0`, negatives kept, trailing zeros removed
// after any decimal point, integer-valued doubles emitted without the
// decimal. Matches KiCad's `FormatDouble` behavior closely enough for
// round-trip.
std::string format_double(double d);

// --- Convenience builders (used by writers throughout the tree) -----

SExprPtr list(std::string head);
SExprPtr list(std::string head, std::initializer_list<SExprPtr> kids);

} // namespace sexpr
