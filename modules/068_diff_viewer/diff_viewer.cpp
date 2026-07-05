#include "diff_viewer.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace diff_viewer {
namespace {

std::vector<std::string> split_lines(std::string_view text) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : text) {
        if (c == '\n') { out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

// LCS-table backtrack producing (op, before_ix, after_ix) trace.
// Op: 0 = Same, 1 = Removed (from `a`), 2 = Added (from `b`).
struct Step { int op; size_t i; size_t j; };

std::vector<Step> lcs_trace(const std::vector<std::string> & a,
                            const std::vector<std::string> & b) {
    const size_t m = a.size(), n = b.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            dp[i + 1][j + 1] = (a[i] == b[j])
                ? dp[i][j] + 1
                : std::max(dp[i + 1][j], dp[i][j + 1]);
    std::vector<Step> rev;
    size_t i = m, j = n;
    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) { rev.push_back({0, i - 1, j - 1}); --i; --j; }
        else if (dp[i - 1][j] >= dp[i][j - 1]) { rev.push_back({1, i - 1, 0}); --i; }
        else                                    { rev.push_back({2, 0, j - 1}); --j; }
    }
    while (i > 0) { rev.push_back({1, --i, 0}); }
    while (j > 0) { rev.push_back({2, 0, --j}); }
    std::reverse(rev.begin(), rev.end());
    return rev;
}

std::string clip(std::string_view s, size_t w) {
    if (s.size() <= w) return std::string(s);
    if (w <= 3) return std::string(s.substr(0, w));
    return std::string(s.substr(0, w - 3)) + "...";
}

std::string pad_right(std::string_view s, size_t w) {
    std::string out = clip(s, w);
    if (out.size() < w) out.append(w - out.size(), ' ');
    return out;
}

}

void init()     {}
void shutdown() {}

Diff diff_text(std::string_view before, std::string_view after) {
    Diff d;
    auto a = split_lines(before);
    auto b = split_lines(after);
    auto trace = lcs_trace(a, b);
    size_t bn = 1, an = 1;
    for (const auto & s : trace) {
        Line l;
        if (s.op == 0) {
            l.kind = Line::Kind::Same;
            l.text = a[s.i];
            l.before_line = bn++;
            l.after_line  = an++;
        } else if (s.op == 1) {
            l.kind = Line::Kind::Removed;
            l.text = a[s.i];
            l.before_line = bn++;
        } else {
            l.kind = Line::Kind::Added;
            l.text = b[s.j];
            l.after_line = an++;
        }
        d.lines.push_back(std::move(l));
    }
    return d;
}

std::string render_inline(const Diff & d) {
    std::ostringstream os;
    for (const auto & l : d.lines) {
        switch (l.kind) {
            case Line::Kind::Same:    os << "  ";    break;
            case Line::Kind::Removed: os << "- ";    break;
            case Line::Kind::Added:   os << "+ ";    break;
        }
        os << l.text << "\n";
    }
    return os.str();
}

std::string render_side_by_side(const Diff & d, size_t w) {
    std::ostringstream os;
    // Walk the trace and group Removed/Added pairs into a single row so
    // the columns line up visually.
    size_t i = 0;
    while (i < d.lines.size()) {
        if (d.lines[i].kind == Line::Kind::Same) {
            os << pad_right(d.lines[i].text, w) << " | " << pad_right(d.lines[i].text, w) << "\n";
            ++i;
            continue;
        }
        // Walk a run of mixed Removed/Added lines, pairing left-side
        // Removed with right-side Added in order of appearance.
        std::vector<std::string> rem, add;
        while (i < d.lines.size() && d.lines[i].kind != Line::Kind::Same) {
            if (d.lines[i].kind == Line::Kind::Removed) rem.push_back(d.lines[i].text);
            else                                        add.push_back(d.lines[i].text);
            ++i;
        }
        size_t n = std::max(rem.size(), add.size());
        for (size_t k = 0; k < n; ++k) {
            std::string l = k < rem.size() ? std::string("< ") + rem[k] : std::string(w, ' ');
            std::string r = k < add.size() ? std::string("> ") + add[k] : std::string(w, ' ');
            os << pad_right(l, w) << " | " << pad_right(r, w) << "\n";
        }
    }
    return os.str();
}

Stats stats(const Diff & d) {
    Stats s;
    for (const auto & l : d.lines) {
        switch (l.kind) {
            case Line::Kind::Same:    ++s.same;    break;
            case Line::Kind::Removed: ++s.removed; break;
            case Line::Kind::Added:   ++s.added;   break;
        }
    }
    return s;
}

MergeResult three_way_merge(std::string_view base_v, std::string_view left_v,
                            std::string_view right_v,
                            std::string_view left_label,
                            std::string_view right_label) {
    MergeResult r;
    auto B = split_lines(base_v);
    auto L = split_lines(left_v);
    auto R = split_lines(right_v);

    // Walk lockstep across the three sides using two LCS traces (base
    // vs left, base vs right). Region semantics: for each base line,
    // see whether left agrees, right agrees, both, or neither.
    auto trL = lcs_trace(B, L);   // op==1 means "removed in left"
    auto trR = lcs_trace(B, R);

    // Convert each trace into a per-base-line "changed?" stream of
    // chunks. Easier: produce two parallel vectors L_for_base[i] and
    // R_for_base[i] giving the corresponding text (or null if removed).
    auto build_aligned = [&](const std::vector<std::string> & B,
                             const std::vector<std::string> & X,
                             const std::vector<Step> & tr) {
        std::vector<std::vector<std::string>> insertions(B.size() + 1);
        std::vector<bool> removed(B.size(), false);
        size_t bi = 0;
        for (const auto & s : tr) {
            if (s.op == 0)      { ++bi; }
            else if (s.op == 1) { removed[s.i] = true; ++bi; }
            else                { insertions[bi].push_back(X[s.j]); }
        }
        (void)B; (void)X;
        return std::make_pair(insertions, removed);
    };
    auto [insL, remL] = build_aligned(B, L, trL);
    auto [insR, remR] = build_aligned(B, R, trR);

    std::ostringstream os;
    auto emit_left  = [&](const std::vector<std::string> & lines) {
        for (const auto & s : lines) os << s << "\n";
    };
    auto emit_conflict = [&](const std::vector<std::string> & left_block,
                             const std::vector<std::string> & right_block) {
        os << "<<<<<<< " << left_label << "\n";
        for (const auto & s : left_block)  os << s << "\n";
        os << "=======\n";
        for (const auto & s : right_block) os << s << "\n";
        os << ">>>>>>> " << right_label << "\n";
        ++r.conflict_count;
    };

    auto blocks_equal = [](const std::vector<std::string> & a,
                           const std::vector<std::string> & b) {
        return a == b;
    };

    // Handle insertions before the first base line.
    if (insL[0] == insR[0]) emit_left(insL[0]);
    else                    emit_conflict(insL[0], insR[0]);

    for (size_t i = 0; i < B.size(); ++i) {
        // Decide the fate of base line i.
        if (!remL[i] && !remR[i]) {
            // Both keep the line.
            os << B[i] << "\n";
        } else if (remL[i] && remR[i]) {
            // Both removed the line.
        } else if (remL[i] && !remR[i]) {
            // Left removed, right kept -> accept the left side (deletion).
        } else if (!remL[i] && remR[i]) {
            // Right removed, left kept.
        }
        // Insertions immediately after base line i.
        const auto & il = insL[i + 1];
        const auto & ir = insR[i + 1];
        if (blocks_equal(il, ir)) emit_left(il);
        else if (il.empty())      emit_left(ir);
        else if (ir.empty())      emit_left(il);
        else                      emit_conflict(il, ir);
    }

    r.text = os.str();
    return r;
}

}
