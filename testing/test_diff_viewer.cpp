// Smoke test for 068_diff_viewer.

#include "test_runner.hpp"
#include "../modules/068_diff_viewer/diff_viewer.hpp"

namespace {

testing::TestOutcome run() {
    diff_viewer::init();

    std::string a = "alpha\nbeta\ngamma\ndelta\n";
    std::string b = "alpha\nBETA\ngamma\ndelta\nepsilon\n";

    auto d  = diff_viewer::diff_text(a, b);
    auto st = diff_viewer::stats(d);
    if (st.added != 2 || st.removed != 1)
        return testing::fail("stats: expected +2/-1, got +" +
                             std::to_string(st.added) + "/-" + std::to_string(st.removed));

    auto inl = diff_viewer::render_inline(d);
    if (inl.find("- beta")    == std::string::npos)
        return testing::fail("inline missing - beta");
    if (inl.find("+ BETA")    == std::string::npos)
        return testing::fail("inline missing + BETA");
    if (inl.find("+ epsilon") == std::string::npos)
        return testing::fail("inline missing + epsilon");

    auto sbs = diff_viewer::render_side_by_side(d, 20);
    if (sbs.find("< beta") == std::string::npos ||
        sbs.find("> BETA") == std::string::npos)
        return testing::fail("side-by-side missing change pair");
    if (sbs.find(" | ") == std::string::npos)
        return testing::fail("side-by-side missing column separator");

    // Three-way merge: independent changes on either side should
    // resolve without conflict.
    std::string base   = "alpha\nbeta\ngamma\n";
    std::string left   = "alpha\nBETA\ngamma\n";       // changed beta -> BETA
    std::string right  = "alpha\nbeta\ngamma\ndelta\n";// added delta line
    auto m = diff_viewer::three_way_merge(base, left, right);
    if (m.conflict_count != 0)
        return testing::fail("expected 0 conflicts on independent edits, got " +
                             std::to_string(m.conflict_count));
    if (m.text.find("BETA")  == std::string::npos)
        return testing::fail("merge dropped LEFT change");
    if (m.text.find("delta") == std::string::npos)
        return testing::fail("merge dropped RIGHT change");

    // Now a true conflict: both sides add a different line at the end.
    std::string l2 = "alpha\nbeta\ngamma\nL-tail\n";
    std::string r2 = "alpha\nbeta\ngamma\nR-tail\n";
    auto m2 = diff_viewer::three_way_merge(base, l2, r2, "LEFT", "RIGHT");
    if (m2.conflict_count != 1)
        return testing::fail("expected 1 conflict on parallel adds");
    if (m2.text.find("<<<<<<< LEFT") == std::string::npos ||
        m2.text.find("=======")      == std::string::npos ||
        m2.text.find(">>>>>>> RIGHT") == std::string::npos)
        return testing::fail("conflict markers missing");
    if (m2.text.find("L-tail") == std::string::npos ||
        m2.text.find("R-tail") == std::string::npos)
        return testing::fail("conflict body missing one side");

    diff_viewer::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "diff_viewer",
    "068_diff_viewer: LCS diff + inline / side-by-side + 3-way merge w/ conflicts",
    &run);

}
