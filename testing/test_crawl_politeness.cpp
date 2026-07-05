// Smoke test for 047_crawl_politeness.

#include "test_runner.hpp"
#include "../modules/047_crawl_politeness/crawl_politeness.hpp"

namespace {

testing::TestOutcome run() {
    crawl_politeness::init();
    crawl_politeness::reset();

    const int64_t t0 = 1'700'000'000;
    auto c0 = crawl_politeness::check_budget("example.com", 5, 2, t0);
    if (!c0.allowed) return testing::fail("first check_budget refused");

    // 5 requests fill the per-minute budget at t0 + 0, +1, +2, +3, +4 seconds.
    for (int i = 0; i < 5; ++i)
        crawl_politeness::track("example.com", 200, 1024, 50, t0 + i);

    auto c1 = crawl_politeness::check_budget("example.com", 5, 0, t0 + 10);
    if (c1.allowed) return testing::fail("budget enforced wrong");
    if (c1.retry_after_seconds <= 0) return testing::fail("retry_after not set");

    // 60s later the window has emptied; allowed again.
    auto c2 = crawl_politeness::check_budget("example.com", 5, 0, t0 + 70);
    if (!c2.allowed) return testing::fail("budget did not reset after 60s");

    // min_gap_seconds enforcement.
    crawl_politeness::reset();
    crawl_politeness::track("a.com", 200, 0, 10, t0);
    auto c3 = crawl_politeness::check_budget("a.com", 100, 5, t0 + 1);
    if (c3.allowed || c3.reason != "under min_gap_seconds")
        return testing::fail("min_gap not enforced");

    // 4xx and 5xx counters.
    crawl_politeness::reset();
    crawl_politeness::track("a.com", 404, 0, 10, t0);
    crawl_politeness::track("a.com", 503, 0, 10, t0 + 1);
    auto s = crawl_politeness::stats("a.com");
    if (s.errors_4xx != 1 || s.errors_5xx != 1)
        return testing::fail("error counters wrong");

    // Blacklist short-circuits regardless of budget.
    crawl_politeness::mark_blacklisted("bad.com");
    auto cb = crawl_politeness::check_budget("bad.com", 1000, 0, t0);
    if (cb.allowed) return testing::fail("blacklist did not block");
    crawl_politeness::unmark_blacklisted("bad.com");
    auto cu = crawl_politeness::check_budget("bad.com", 1000, 0, t0);
    if (!cu.allowed) return testing::fail("unblacklist did not restore");

    // report() returns every host that has been touched.
    auto rep = crawl_politeness::report();
    if (rep.find("a.com") == rep.end())
        return testing::fail("report missing tracked host");

    crawl_politeness::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "crawl_politeness",
    "047_crawl_politeness: per-host rate window + min-gap + blacklist + report",
    &run);

}
