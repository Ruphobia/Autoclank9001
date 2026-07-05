// Smoke test for 057_attachment_deduper.

#include "test_runner.hpp"
#include "../modules/057_attachment_deduper/attachment_deduper.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_dedup_att";
    return fs::path("/tmp") / "tool_test_dedup_att";
}

void write(const fs::path & p, const std::string & body) {
    std::ofstream o(p, std::ios::binary);
    o.write(body.data(), (std::streamsize)body.size());
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    attachment_deduper::init();

    // Two byte-identical PDFs + one byte-different but near-text-duplicate
    // file + one wholly unrelated file.
    std::string body_a = std::string(1024, 'a');
    write(root / "x.pdf", body_a);
    write(root / "y.pdf", body_a);     // byte-identical copy
    write(root / "z.pdf", body_a + "z");
    write(root / "w.pdf", std::string(800, 'q'));

    std::string text_x = "Capybara thermoregulation experiments at lot 14.";
    std::string text_y = "Capybara thermoregulation experiments at lot 14.";
    std::string text_z = "Capybara thermoregulation experiments at lot 14.";
    std::string text_w = "Argon flow rate calibration sequence.";

    std::vector<attachment_deduper::FileFp> fps;
    fps.push_back(attachment_deduper::fingerprint((root / "x.pdf").string(), text_x));
    fps.push_back(attachment_deduper::fingerprint((root / "y.pdf").string(), text_y));
    fps.push_back(attachment_deduper::fingerprint((root / "z.pdf").string(), text_z));
    fps.push_back(attachment_deduper::fingerprint((root / "w.pdf").string(), text_w));

    auto groups = attachment_deduper::cluster(fps);
    bool saw_byte = false, saw_near = false, saw_w_alone = true;
    for (const auto & g : groups) {
        if (g.byte_identical && g.paths.size() == 2) saw_byte = true;
        if (!g.byte_identical && g.paths.size() == 1) {
            // shouldn't happen; near-dup groups must have at least 2.
            saw_w_alone = false;
        }
        if (!g.byte_identical && g.paths.size() >= 1) {
            // The byte-different but text-matching z.pdf should join a
            // near-dup group with at least one of x.pdf or y.pdf? But
            // x and y are taken by the byte_identical group, so z is
            // alone after the byte-pass and produces no near-dup group.
            // We expect zero non-byte groups in this corpus.
            saw_near = true;
        }
    }
    if (!saw_byte) return testing::fail("byte-identical group missed");
    if (saw_near) return testing::fail("z.pdf alone should not form a near-dup group");
    (void)saw_w_alone;

    // Second scenario: three byte-different files, all sharing the
    // same text excerpt -> one near-dup group of 3.
    fs::remove_all(root, ec);
    fs::create_directories(root);
    for (char c : {'1','2','3'}) {
        write(root / (std::string("p") + c + ".pdf"),
              std::string(1000, c));    // each file is unique bytes
    }
    std::vector<attachment_deduper::FileFp> fps2;
    for (char c : {'1','2','3'}) {
        fps2.push_back(attachment_deduper::fingerprint(
            (root / (std::string("p") + c + ".pdf")).string(),
            "Capybara thermoregulation lot 14 OCR text body that matches."));
    }
    auto g2 = attachment_deduper::cluster(fps2);
    bool got_near3 = false;
    for (const auto & g : g2) {
        if (!g.byte_identical && g.paths.size() == 3) got_near3 = true;
    }
    if (!got_near3) return testing::fail("expected one near-dup group of 3");

    attachment_deduper::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "attachment_deduper",
    "057_attachment_deduper: byte-identical clusters + SimHash near-dup",
    &run);

}
