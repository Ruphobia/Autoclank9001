#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Site Crawler: orchestrator that composes 040_robots_sitemap,
// 044_dedup_ingest, 046_link_graph, 047_crawl_politeness, and
// 049_page_triage into a polite, deduplicating BFS walk. The actual
// HTTP fetcher is injected so this layer has no networking
// dependency; a real run uses libcurl behind the Fetcher, tests use
// an in-memory map.
namespace site_crawler {

struct FetchResult {
    int          status        = 0;
    std::string  body;
    std::string  content_type;
    std::string  final_url;        // after redirects
};

using Fetcher = std::function<FetchResult(const std::string & url)>;

struct CrawlConfig {
    size_t      max_pages              = 200;
    int         max_depth              = 3;
    int         max_per_minute_per_host = 30;
    int         min_gap_seconds        = 1;
    bool        obey_robots            = true;
    bool        triage_drop            = true;
    double      quality_floor          = 0.20;
    std::string user_agent             = "tool-crawler";
    bool        stay_on_seed_hosts     = true;
};

struct CrawlReport {
    std::vector<std::string>           visited;
    std::vector<std::string>           dropped;          // triage said drop
    std::vector<std::string>           blocked_robots;   // robots said no
    std::vector<std::string>           dead;             // 4xx/5xx
    std::unordered_map<std::string, int> depth_by_url;
};

void init();
void shutdown();

CrawlReport run(const std::vector<std::string> & seeds,
                const Fetcher & fetch,
                const CrawlConfig & cfg);

}
