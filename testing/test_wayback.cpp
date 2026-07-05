// Smoke test for 052_wayback. The HttpClient is in-memory; the test
// hands back a canned CDX JSON response and a per-snapshot body.

#include "test_runner.hpp"
#include "../modules/052_wayback/wayback.hpp"

namespace {

testing::TestOutcome run() {
    wayback::init();

    api_harvester::HttpClient client = [&](const api_harvester::HttpRequest & r) {
        api_harvester::HttpResponse resp;
        resp.status = 200;
        if (r.url.find("/cdx/search/cdx") != std::string::npos) {
            resp.body =
                R"([)"
                R"(["urlkey","timestamp","original","mimetype","statuscode","digest","length"],)"
                R"(["com,example)/", "20200101000000", "https://example.com/", "text/html", "200", "d1", "100"],)"
                R"(["com,example)/", "20220615123000", "https://example.com/", "text/html", "200", "d2", "120"])"
                R"(])";
        } else if (r.url.find("/web/20200101000000/") != std::string::npos) {
            resp.body = "snapshot body 2020";
        } else if (r.url.find("/web/20220615123000/") != std::string::npos) {
            resp.body = "snapshot body 2022";
        } else {
            resp.status = 404;
        }
        return resp;
    };

    auto snaps = wayback::snapshots("https://example.com/", client);
    if (snaps.size() != 2) return testing::fail("snapshot count != 2");
    if (snaps[0].timestamp != "20200101000000")
        return testing::fail("first timestamp wrong");
    if (snaps[1].timestamp != "20220615123000")
        return testing::fail("second timestamp wrong");

    auto u = wayback::snapshot_url(snaps[1]);
    if (u != "https://web.archive.org/web/20220615123000/https://example.com/")
        return testing::fail("snapshot_url wrong: " + u);

    auto body0 = wayback::fetch_snapshot(snaps[0], client);
    if (!body0 || *body0 != "snapshot body 2020")
        return testing::fail("snapshot 0 fetch wrong");

    auto body1 = wayback::fetch_snapshot(snaps[1], client);
    if (!body1 || *body1 != "snapshot body 2022")
        return testing::fail("snapshot 1 fetch wrong");

    // 4xx -> nullopt.
    api_harvester::HttpClient dead = [&](const api_harvester::HttpRequest &) {
        api_harvester::HttpResponse r; r.status = 404; return r;
    };
    if (wayback::fetch_snapshot(snaps[0], dead))
        return testing::fail("4xx fetch returned a body");

    if (!wayback::cdx_query_url("https://example.com/", "exact", 50).empty()
        != true)
        return testing::fail("cdx_query_url empty");

    wayback::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "wayback",
    "052_wayback: CDX snapshot listing + replay-URL build + snapshot fetch",
    &run);

}
