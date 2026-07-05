// Smoke test for 018_artifact_safety: record a hazard, append a
// mitigation and an incident, verify file content + counts.

#include "test_runner.hpp"
#include "../modules/018_artifact_safety/safety_folder.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_safety_folder";
    return fs::path("/tmp") / "tool_test_safety_folder";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    safety_folder::init();
    if (safety_folder::has_been_explained("hf_skin", root.string()))
        return testing::fail("clean dir reports already-explained");

    safety_folder::record_hazard("hf_skin", "hydrofluoric acid",
        "Fluoride penetrates skin and chelates serum calcium.",
        root.string());
    if (!safety_folder::has_been_explained("hf_skin", root.string()))
        return testing::fail("record_hazard didn't create the file");

    // Idempotent re-record: must not wipe body. Touch should update.
    safety_folder::record_hazard("hf_skin", "hydrofluoric acid",
        "Fluoride penetrates skin and chelates serum calcium.",
        root.string());

    safety_folder::log_mitigation("hf_skin", "Calcium gluconate gel staged at bench", root.string());
    safety_folder::log_mitigation("hf_skin", "Switched to dilute buffered etch (BOE)", root.string());
    safety_folder::log_incident  ("hf_skin", "Pinhole splash on glove; no skin contact", root.string());

    auto entry = safety_folder::read("hf_skin", root.string());
    if (!entry) return testing::fail("read returned nullopt");
    if (entry->mitigation_count != 2)
        return testing::fail("expected 2 mitigations, got " + std::to_string(entry->mitigation_count));
    if (entry->incident_count != 1)
        return testing::fail("expected 1 incident, got " + std::to_string(entry->incident_count));
    if (entry->body.find("Calcium gluconate") == std::string::npos)
        return testing::fail("body missing mitigation text");
    if (entry->body.find("Pinhole splash") == std::string::npos)
        return testing::fail("body missing incident text");
    if (entry->body.find("Mechanism") == std::string::npos)
        return testing::fail("body missing Mechanism section");
    if (entry->title != "hydrofluoric acid (`hf_skin`)")
        return testing::fail("title parsed wrong: '" + entry->title + "'");

    // Log against a fresh hazard with no prior record: should auto-create.
    safety_folder::log_mitigation("co_combustion", "CO alarm installed", root.string());
    auto co = safety_folder::read("co_combustion", root.string());
    if (!co) return testing::fail("auto-create on log_mitigation failed");
    if (co->mitigation_count != 1) return testing::fail("auto-created entry missing mitigation");

    auto known = safety_folder::list_known(root.string());
    if (known.size() != 2) return testing::fail("list_known size wrong: " + std::to_string(known.size()));

    safety_folder::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "artifact_safety",
    "018_artifact_safety: per-hazard markdown file accumulating mitigations / incidents",
    &run);

}
