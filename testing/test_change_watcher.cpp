// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 045_change_watcher.

#include "test_runner.hpp"
#include "../modules/045_change_watcher/change_watcher.hpp"

namespace {

testing::TestOutcome run() {
    change_watcher::init();

    std::string before =
        "Header\n"
        "csrf_token: abc123\n"
        "main paragraph stable\n"
        "served at 2026-06-29T10:00:00Z\n"
        "footer\n";
    std::string after =
        "Header\n"
        "csrf_token: def987\n"
        "main paragraph stable\n"
        "served at 2026-06-29T10:01:42Z\n"
        "footer\n";

    // Without masks, the two differ on token + timestamp lines.
    auto r0 = change_watcher::compare(before, after);
    if (!r0.changed) return testing::fail("noisy compare reported no change");

    // With masks neutralizing both, the compare should report unchanged.
    std::vector<change_watcher::NoiseMask> masks = {
        {"csrf_token:\\s*[a-z0-9]+",  "csrf_token: <MASKED>"},
        {"served at \\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z",
         "served at <MASKED>"},
    };
    auto r1 = change_watcher::compare(before, after, masks);
    if (r1.changed) return testing::fail("masked compare still flagged change");
    if (r1.similarity < 0.99) return testing::fail("masked similarity not ~1");
    if (r1.masked_after.find("<MASKED>") == std::string::npos)
        return testing::fail("masks did not fire");

    // Real change in the middle should still be flagged.
    std::string real_after =
        "Header\n"
        "csrf_token: xyz999\n"
        "main paragraph REWRITTEN\n"
        "served at 2026-06-29T10:02:00Z\n"
        "footer\n";
    auto r2 = change_watcher::compare(before, real_after, masks);
    if (!r2.changed) return testing::fail("real change not flagged");
    bool saw_removed = false, saw_added = false;
    for (const auto & d : r2.diff) {
        if (d.kind == change_watcher::LineDiff::Kind::Removed &&
            d.text.find("stable") != std::string::npos) saw_removed = true;
        if (d.kind == change_watcher::LineDiff::Kind::Added &&
            d.text.find("REWRITTEN") != std::string::npos) saw_added = true;
    }
    if (!saw_removed || !saw_added)
        return testing::fail("diff did not surface the rewrite");

    change_watcher::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "change_watcher",
    "045_change_watcher: masked line-diff that ignores rotating noise",
    &run);

}
