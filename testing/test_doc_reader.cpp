// Smoke test for 015_doc_reader: open an index in a scratch root,
// ingest two small files, search for a phrase, confirm the hit.

#include "test_runner.hpp"
#include "../modules/015_doc_reader/doc_reader.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_doc_reader";
    return fs::path("/tmp") / "tool_test_doc_reader";
}

void write(const fs::path & p, const std::string & body) {
    std::ofstream o(p, std::ios::binary);
    o << body;
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    doc_reader::open(root.string());

    fs::path src_dir = root / "docs";
    fs::create_directories(src_dir);
    fs::path txt  = src_dir / "alpha.txt";
    fs::path html = src_dir / "beta.html";
    fs::path md   = src_dir / "gamma.md";

    write(txt,  "The capybara is the largest rodent. It lives near water in South America.\n");
    write(html, "<html><body><h1>Datasheet</h1>"
                "<p>This Zener diode operates at <b>5.1 volts</b> reverse breakdown.</p>"
                "</body></html>\n");
    write(md,   "# Project capybara\n\n"
                "Capybaras prefer warm climates and are surprisingly fast swimmers.\n");

    if (!doc_reader::ingest(txt.string()))  return testing::fail("ingest txt rejected");
    if (!doc_reader::ingest(html.string())) return testing::fail("ingest html rejected");
    if (!doc_reader::ingest(md.string()))   return testing::fail("ingest md rejected");

    auto docs = doc_reader::list();
    if (docs.size() != 3) return testing::fail("expected 3 docs, got " + std::to_string(docs.size()));

    auto hits = doc_reader::search("capybara", 5);
    if (hits.empty()) return testing::fail("no hits for 'capybara'");
    bool saw_txt = false, saw_md = false, saw_html = false;
    for (const auto & h : hits) {
        if (h.document.path == txt.string())  saw_txt  = true;
        if (h.document.path == md.string())   saw_md   = true;
        if (h.document.path == html.string()) saw_html = true;
    }
    if (!saw_txt || !saw_md)
        return testing::fail("capybara hits missed expected docs");
    if (saw_html)
        return testing::fail("capybara hit on html doc (false positive)");

    auto zener = doc_reader::search("zener AND breakdown", 5);
    if (zener.empty()) return testing::fail("no hits for zener");
    if (zener[0].document.path != html.string())
        return testing::fail("zener top hit wasn't the html doc");
    if (zener[0].snippet.find("[[") == std::string::npos)
        return testing::fail("snippet missing FTS highlight markers");

    doc_reader::reset();
    if (!doc_reader::list().empty()) return testing::fail("reset did not clear docs");

    doc_reader::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "doc_reader",
    "015_doc_reader: ingest txt/md/html, FTS5 search, snippet, reset",
    &run);

}
