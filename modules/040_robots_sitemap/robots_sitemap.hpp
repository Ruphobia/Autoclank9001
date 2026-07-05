#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Robots and Sitemap Inspector: parses robots.txt directives and
// sitemap.xml / sitemap index files. Pure-parser surface; no
// networking. The Site Crawler module wires this to actual fetches.
namespace robots_sitemap {

struct RobotsGroup {
    std::vector<std::string> user_agents;
    std::vector<std::string> allow;
    std::vector<std::string> disallow;
    int                      crawl_delay_seconds = 0;     // 0 = unspecified
};

struct RobotsRules {
    std::vector<RobotsGroup> groups;
    std::vector<std::string> sitemaps;
    std::vector<std::string> host_hints;     // any "Host:" directives encountered
};

struct SitemapUrl {
    std::string loc;
    std::string lastmod;
    std::string changefreq;
    std::string priority;
};

void init();
void shutdown();

RobotsRules parse_robots(std::string_view body);

// Resolve the rules for `user_agent` against `url_path` (path part of
// the URL, e.g. "/foo/bar"). Implements the standard "longest path
// rule wins, allow beats disallow on tie" algorithm.
bool is_allowed(const RobotsRules & rules,
                std::string_view user_agent,
                std::string_view url_path);

// Crawl delay declared for a given user agent (or *), or 0 if none.
int crawl_delay_for(const RobotsRules & rules, std::string_view user_agent);

// Parse a sitemap.xml urlset. Returns one entry per <url><loc>...</loc></url>.
std::vector<SitemapUrl> parse_sitemap_xml(std::string_view body);

// Parse a sitemap index (sitemapindex root). Returns the sub-sitemap URLs.
std::vector<std::string> parse_sitemap_index_xml(std::string_view body);

}
