// Smoke test for 020_artifact_ip.

#include "test_runner.hpp"
#include "../modules/020_artifact_ip/ip_folder.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_ip_folder";
    return fs::path("/tmp") / "tool_test_ip_folder";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    ip_folder::init();
    if (ip_folder::exists("widget_drive", root.string()))
        return testing::fail("clean dir reports already-disclosed");

    ip_folder::record_disclosure(
        "widget_drive", "Magnetically-coupled widget drive",
        "A drive train coupling that transfers torque across a hermetic seal.",
        root.string());

    ip_folder::add_claim("widget_drive",
        "A magnetic coupling comprising opposing axial arrays of rare-earth magnets...",
        root.string());
    ip_folder::add_claim("widget_drive",
        "The coupling of claim 1 wherein the seal is fabricated from inconel.",
        root.string());
    ip_folder::add_prior_art("widget_drive",
        "US 4,123,456 (1980) Bauer; comparable axial magnet topology",
        root.string());
    ip_folder::add_supporting("widget_drive",
        "git: 4fd4dd6 (test fixture) - parametric magnet array sweep results",
        root.string());
    ip_folder::set_status("widget_drive", "defensive-published", root.string());

    auto d = ip_folder::read("widget_drive", root.string());
    if (!d) return testing::fail("read returned nullopt");
    if (d->claims.size()     != 2) return testing::fail("claims count wrong");
    if (d->prior_art.size()  != 1) return testing::fail("prior art count wrong");
    if (d->supporting.size() != 1) return testing::fail("supporting count wrong");
    if (d->status != "defensive-published")
        return testing::fail("status not updated: '" + d->status + "'");
    if (d->title != "Magnetically-coupled widget drive (`widget_drive`)")
        return testing::fail("title not parsed");

    // Auto-create on add_claim against unknown id.
    ip_folder::add_claim("other_id", "first claim", root.string());
    auto d2 = ip_folder::read("other_id", root.string());
    if (!d2 || d2->claims.size() != 1) return testing::fail("auto-create on add_claim broken");

    if (ip_folder::list(root.string()).size() != 2)
        return testing::fail("list count wrong");

    ip_folder::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "artifact_ip",
    "020_artifact_ip: disclosure + claims/prior art/supporting/status",
    &run);

}
