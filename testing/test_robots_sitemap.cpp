// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 040_robots_sitemap.

#include "test_runner.hpp"
#include "../modules/040_robots_sitemap/robots_sitemap.hpp"

namespace {

const char * kRobots = R"ROB(
# Comments are fine
User-agent: *
Disallow: /private/
Allow: /private/public-bits/
Crawl-delay: 2

User-agent: tool-bot
Disallow: /
Allow: /api/

Sitemap: https://example.com/sitemap.xml
Sitemap: https://example.com/sitemap-news.xml
Host: example.com
)ROB";

const char * kSitemap = R"SM(
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
  <url>
    <loc>https://example.com/a</loc>
    <lastmod>2026-06-29</lastmod>
    <changefreq>weekly</changefreq>
    <priority>0.8</priority>
  </url>
  <url>
    <loc>https://example.com/b</loc>
    <lastmod>2026-06-28</lastmod>
  </url>
</urlset>
)SM";

const char * kSitemapIndex = R"SMI(
<?xml version="1.0" encoding="UTF-8"?>
<sitemapindex>
  <sitemap><loc>https://example.com/s1.xml</loc></sitemap>
  <sitemap><loc>https://example.com/s2.xml</loc></sitemap>
</sitemapindex>
)SMI";

testing::TestOutcome run() {
    robots_sitemap::init();
    auto r = robots_sitemap::parse_robots(kRobots);

    if (r.groups.size() != 2)
        return testing::fail("expected 2 groups, got " + std::to_string(r.groups.size()));
    if (r.sitemaps.size() != 2) return testing::fail("sitemap count != 2");
    if (r.host_hints.size() != 1) return testing::fail("host hints not captured");

    // * group: /private/ is disallowed except /private/public-bits/.
    if (robots_sitemap::is_allowed(r, "Mozilla", "/private/secret.html"))
        return testing::fail("disallow /private/ did not bite");
    if (!robots_sitemap::is_allowed(r, "Mozilla", "/private/public-bits/ok.html"))
        return testing::fail("allow longer match did not win");
    if (!robots_sitemap::is_allowed(r, "Mozilla", "/index.html"))
        return testing::fail("default allow broken");

    // tool-bot group: blanket disallow except /api/.
    if (robots_sitemap::is_allowed(r, "tool-bot", "/foo"))
        return testing::fail("tool-bot blanket disallow failed");
    if (!robots_sitemap::is_allowed(r, "tool-bot", "/api/list"))
        return testing::fail("tool-bot allow /api/ failed");

    if (robots_sitemap::crawl_delay_for(r, "Mozilla") != 2)
        return testing::fail("Mozilla crawl delay not 2");

    auto urls = robots_sitemap::parse_sitemap_xml(kSitemap);
    if (urls.size() != 2) return testing::fail("sitemap url count != 2");
    if (urls[0].loc != "https://example.com/a") return testing::fail("sitemap loc wrong");
    if (urls[0].priority != "0.8") return testing::fail("sitemap priority wrong");

    auto idx = robots_sitemap::parse_sitemap_index_xml(kSitemapIndex);
    if (idx.size() != 2 || idx[0] != "https://example.com/s1.xml")
        return testing::fail("sitemap index parse wrong");

    robots_sitemap::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "robots_sitemap",
    "040_robots_sitemap: robots.txt + sitemap.xml + sitemap index parsers",
    &run);

}
