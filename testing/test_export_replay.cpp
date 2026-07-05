// Smoke test for 053_export_replay.

#include "test_runner.hpp"
#include "../modules/053_export_replay/export_replay.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_export_replay";
    return fs::path("/tmp") / "tool_test_export_replay";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    export_replay::init();

    std::vector<export_replay::Record> recs;
    {
        export_replay::Record r;
        r.warc_type    = "warcinfo";
        r.date_iso     = "2026-06-29T15:00:00Z";
        r.record_id    = "<urn:uuid:1>";
        r.content_type = "application/warc-fields";
        r.body         = "software: tool-crawler/0.1\nformat: WARC/1.0\n";
        recs.push_back(r);
    }
    {
        export_replay::Record r;
        r.warc_type    = "response";
        r.date_iso     = "2026-06-29T15:00:01Z";
        r.record_id    = "<urn:uuid:2>";
        r.target_uri   = "https://example.com/a";
        r.content_type = "application/http; msgtype=response";
        r.body         =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><body>alpha</body></html>";
        recs.push_back(r);
    }
    {
        export_replay::Record r;
        r.warc_type    = "resource";
        r.date_iso     = "2026-06-29T15:00:02Z";
        r.record_id    = "<urn:uuid:3>";
        r.target_uri   = "https://example.com/binary";
        r.content_type = "application/octet-stream";
        r.body         = std::string("\x00\x01\x02 binary\xff", 10);
        recs.push_back(r);
    }

    fs::path warc = root / "out.warc";
    size_t   nw   = export_replay::write_warc(recs, warc.string());
    if (nw != 3) return testing::fail("warc write count != 3");

    auto round = export_replay::read_warc(warc.string());
    if (round.size() != 3) return testing::fail("warc read count != 3");
    if (round[1].target_uri != "https://example.com/a")
        return testing::fail("warc round trip lost target uri");
    if (round[1].body.find("alpha") == std::string::npos)
        return testing::fail("warc round trip lost body");
    if (round[2].body.size() != 10)
        return testing::fail("warc round trip lost binary body length: " +
                             std::to_string(round[2].body.size()));

    size_t replayed = 0;
    export_replay::replay_warc(warc.string(),
        [&](const export_replay::Record &){ ++replayed; });
    if (replayed != 3) return testing::fail("replay callback count != 3");

    fs::path jsonl = root / "out.jsonl";
    size_t   nj    = export_replay::write_jsonl(recs, jsonl.string());
    if (nj != 3) return testing::fail("jsonl write count != 3");
    auto round_j = export_replay::read_jsonl(jsonl.string());
    if (round_j.size() != 3) return testing::fail("jsonl read count != 3");
    if (round_j[2].body.size() != 10)
        return testing::fail("jsonl b64 binary round trip wrong");

    export_replay::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "export_replay",
    "053_export_replay: WARC + JSONL write/read + replay callback",
    &run);

}
