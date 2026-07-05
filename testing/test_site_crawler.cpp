// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 050_site_crawler. Uses an in-memory fetcher so the
// crawler can be exercised without a network.

#include "test_runner.hpp"
#include "../modules/050_site_crawler/site_crawler.hpp"

#include <algorithm>
#include <unordered_map>

namespace {

site_crawler::FetchResult make_html(const std::string & body) {
    site_crawler::FetchResult r;
    r.status = 200; r.body = body; r.content_type = "text/html; charset=utf-8";
    return r;
}
site_crawler::FetchResult make_404() {
    site_crawler::FetchResult r;
    r.status = 404; r.body = "not found";
    return r;
}

testing::TestOutcome run() {
    site_crawler::init();

    std::string essay;
    for (int i = 0; i < 80; ++i)
        essay += "This is a clear sentence about science. ";

    std::unordered_map<std::string, site_crawler::FetchResult> corpus;
    corpus["https://example.com/robots.txt"] = []{
        site_crawler::FetchResult r;
        r.status = 200;
        r.body   = "User-agent: tool-crawler\nDisallow: /private/\n";
        return r;
    }();
    corpus["https://example.com/"] = make_html(
        "<html><body><article>" + essay + "</article>"
        "<a href=\"/a\">a</a><a href=\"/b\">b</a>"
        "<a href=\"/private/secret\">no</a>"
        "<a href=\"https://other.com/x\">off-host</a>"
        "</body></html>");
    corpus["https://example.com/a"] = make_html(
        "<html><body><article>" + essay + "</article>"
        "<a href=\"/c\">c</a></body></html>");
    corpus["https://example.com/b"] = make_html(
        "<html><body><article>" + essay + "</article></body></html>");
    corpus["https://example.com/c"] = make_html(
        "<html><body><article>" + essay + "</article></body></html>");
    corpus["https://example.com/private/secret"] = make_html(
        "<html><body>private</body></html>");
    corpus["https://example.com/dead"] = make_404();
    // off-host page omitted entirely; we should NOT fetch it when stay_on_seed_hosts is true.

    site_crawler::Fetcher fetch = [&](const std::string & url) {
        auto it = corpus.find(url);
        if (it == corpus.end()) {
            site_crawler::FetchResult r; r.status = 404; return r;
        }
        return it->second;
    };

    site_crawler::CrawlConfig cfg;
    cfg.max_pages              = 100;
    cfg.max_depth              = 3;
    cfg.max_per_minute_per_host = 1000;     // disable rate-limit pause for the test
    cfg.min_gap_seconds        = 0;
    cfg.user_agent             = "tool-crawler";
    cfg.stay_on_seed_hosts     = true;
    cfg.triage_drop            = true;
    cfg.quality_floor          = 0.10;

    auto rep = site_crawler::run({"https://example.com/"}, fetch, cfg);

    auto visited = [&](const std::string & u) {
        return std::find(rep.visited.begin(), rep.visited.end(), u) != rep.visited.end();
    };
    auto blocked = [&](const std::string & u) {
        return std::find(rep.blocked_robots.begin(), rep.blocked_robots.end(), u) != rep.blocked_robots.end();
    };

    if (!visited("https://example.com/")) return testing::fail("seed not visited (canonical)");
    if (!visited("https://example.com/a")) return testing::fail("/a not visited");
    if (!visited("https://example.com/b")) return testing::fail("/b not visited");
    if (!visited("https://example.com/c")) return testing::fail("/c not visited (depth)");
    if (visited("https://example.com/private/secret"))
        return testing::fail("private leaked through robots");
    if (!blocked("https://example.com/private/secret"))
        return testing::fail("private not in blocked_robots");
    if (visited("https://other.com/x"))
        return testing::fail("off-host leaked through host fence");

    // Re-run with a stricter depth: /c should be cut off.
    cfg.max_depth = 1;
    auto rep2 = site_crawler::run({"https://example.com/"}, fetch, cfg);
    auto visited2 = [&](const std::string & u) {
        return std::find(rep2.visited.begin(), rep2.visited.end(), u) != rep2.visited.end();
    };
    if (visited2("https://example.com/c"))
        return testing::fail("depth limit not honored");

    site_crawler::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "site_crawler",
    "050_site_crawler: BFS crawler honoring robots, host fence, depth, triage",
    &run);

}
