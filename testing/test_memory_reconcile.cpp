// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 036_memory_reconcile.

#include "test_runner.hpp"
#include "../modules/036_memory_reconcile/memory_reconcile.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_reconcile";
    return fs::path("/tmp") / "tool_test_reconcile";
}

void write_jsonl(const fs::path & p,
                 const std::vector<std::string> & cats,
                 const std::vector<std::string> & contents,
                 const std::vector<std::vector<std::string>> & tags) {
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    for (size_t i = 0; i < cats.size(); ++i) {
        o << R"({"category":")" << cats[i]
          << R"(","content":")" << contents[i]
          << R"(","tags":[)";
        for (size_t j = 0; j < tags[i].size(); ++j) {
            if (j) o << ",";
            o << "\"" << tags[i][j] << "\"";
        }
        o << "]}\n";
    }
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    // Fingerprint is deterministic and tag-order-independent.
    auto a = memory_reconcile::fingerprint("fact", "x", {"alpha","beta"});
    auto b = memory_reconcile::fingerprint("fact", "x", {"beta","alpha"});
    if (a != b) return testing::fail("fingerprint not order-independent");
    auto c = memory_reconcile::fingerprint("fact", "y", {"alpha","beta"});
    if (a == c) return testing::fail("different content shares fingerprint");

    fs::path L = root / "left.jsonl";
    fs::path R = root / "right.jsonl";
    fs::path B = root / "base.jsonl";

    // Base: { entry1, entry2 }
    write_jsonl(B,
        {"fact","fact"},
        {"shared one","shared two"},
        {{"common"},{"common"}});
    // Left:  { entry1, entry2 (no, removed), entry3 (new) }
    write_jsonl(L,
        {"fact","fact"},
        {"shared one","left added"},
        {{"common"},{"left"}});
    // Right: { entry1, entry2, entry3 (new same as left), entry4 (new right only) }
    write_jsonl(R,
        {"fact","fact","fact","fact"},
        {"shared one","shared two","left added","right only"},
        {{"common"},{"common"},{"left"},{"right"}});

    auto Sl = memory_reconcile::load_jsonl(L.string());
    auto Sr = memory_reconcile::load_jsonl(R.string());
    auto Sb = memory_reconcile::load_jsonl(B.string());

    if (Sl.size() != 2 || Sr.size() != 4 || Sb.size() != 2)
        return testing::fail("snapshot sizes wrong");

    auto d = memory_reconcile::diff(Sl, Sr);
    if (d.left_only.size() != 0 || d.right_only.size() != 2 || d.common.size() != 2)
        return testing::fail("2-way diff wrong: " +
                             std::to_string(d.left_only.size()) + "/" +
                             std::to_string(d.right_only.size()) + "/" +
                             std::to_string(d.common.size()));

    auto cl = memory_reconcile::classify(Sb, Sl, Sr);
    // unchanged: "shared one" only (in all 3).
    if (cl.unchanged.size() != 1 || cl.unchanged[0].content != "shared one")
        return testing::fail("unchanged set wrong");
    // added_both: "left added" appears in left and right but not base.
    if (cl.added_both.size() != 1 || cl.added_both[0].content != "left added")
        return testing::fail("added_both wrong");
    // added_right only: "right only".
    if (cl.added_right.size() != 1 || cl.added_right[0].content != "right only")
        return testing::fail("added_right wrong");
    // removed_left: "shared two" gone from left but still in base and right.
    if (cl.removed_left.size() != 1 || cl.removed_left[0].content != "shared two")
        return testing::fail("removed_left wrong");

    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "memory_reconcile",
    "036_memory_reconcile: fingerprint + 2-way diff + 3-way classify",
    &run);

}
