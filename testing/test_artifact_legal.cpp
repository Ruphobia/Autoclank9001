// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 019_artifact_legal.

#include "test_runner.hpp"
#include "../modules/019_artifact_legal/legal_folder.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_legal_folder";
    return fs::path("/tmp") / "tool_test_legal_folder";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    legal_folder::init();
    if (legal_folder::has_been_explained("nfa_form4", root.string()))
        return testing::fail("clean dir reports already-explained");

    legal_folder::record_pathway(
        "nfa_form4", "US federal", "26 U.S.C. Chapter 53",
        "ATF Form 4 transfer for NFA-regulated items; $200 tax stamp.",
        root.string());
    if (!legal_folder::has_been_explained("nfa_form4", root.string()))
        return testing::fail("record_pathway didn't create the file");

    legal_folder::log_event("nfa_form4", legal_folder::EventKind::Status,
        "Submitted via SOT dealer", root.string());
    legal_folder::log_event("nfa_form4", legal_folder::EventKind::Fee,
        "$200 tax stamp paid", root.string());
    legal_folder::log_event("nfa_form4", legal_folder::EventKind::Followup,
        "Check eForms portal in 30 days", root.string());
    legal_folder::log_event("nfa_form4", legal_folder::EventKind::Contact,
        "Dealer FFL: ACME Arms, 555-0100", root.string());

    auto e = legal_folder::read("nfa_form4", root.string());
    if (!e) return testing::fail("read returned nullopt");
    if (e->status_count   != 1) return testing::fail("status count wrong");
    if (e->fee_count      != 1) return testing::fail("fee count wrong");
    if (e->followup_count != 1) return testing::fail("followup count wrong");
    if (e->contact_count  != 1) return testing::fail("contact count wrong");
    if (e->jurisdiction != "US federal")    return testing::fail("jurisdiction parse wrong");
    if (e->cite != "26 U.S.C. Chapter 53")  return testing::fail("cite parse wrong");

    // Auto-create on log_event.
    legal_folder::log_event("epa_rcra", legal_folder::EventKind::Note,
        "Generator under SQG threshold for now", root.string());
    auto e2 = legal_folder::read("epa_rcra", root.string());
    if (!e2) return testing::fail("auto-create on log_event failed");

    auto known = legal_folder::list_known(root.string());
    if (known.size() != 2) return testing::fail("list_known size wrong");

    legal_folder::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "artifact_legal",
    "019_artifact_legal: per-pathway markdown accumulating status/fees/followups",
    &run);

}
