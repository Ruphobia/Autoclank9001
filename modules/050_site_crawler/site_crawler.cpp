#include "site_crawler.hpp"

#include "../040_robots_sitemap/robots_sitemap.hpp"
#include "../044_dedup_ingest/dedup_ingest.hpp"
#include "../046_link_graph/link_graph.hpp"
#include "../047_crawl_politeness/crawl_politeness.hpp"
#include "../049_page_triage/page_triage.hpp"
#include "../042_readability/readability.hpp"

#include <algorithm>
#include <chrono>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

namespace site_crawler {
namespace {

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

std::string host_of(std::string_view url) {
    auto colon = url.find(":");
    if (colon == std::string_view::npos) return {};
    if (url.size() < colon + 3 || url[colon+1] != '/' || url[colon+2] != '/') return {};
    auto after = url.substr(colon + 3);
    auto end = after.find_first_of("/?#");
    std::string h{after.substr(0, end == std::string_view::npos ? after.size() : end)};
    auto cp = h.find(':');
    if (cp != std::string::npos) h.erase(cp);
    std::transform(h.begin(), h.end(), h.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return h;
}
std::string scheme_of(std::string_view url) {
    auto colon = url.find(":");
    if (colon == std::string_view::npos) return {};
    return std::string{url.substr(0, colon)};
}
std::string path_of(std::string_view url) {
    auto colon = url.find(":");
    if (colon == std::string_view::npos) return "/";
    if (url.size() < colon + 3) return "/";
    auto after = url.substr(colon + 3);
    auto p = after.find('/');
    if (p == std::string_view::npos) return "/";
    auto q = after.find_first_of("?#", p);
    std::string out{after.substr(p, q == std::string_view::npos ? after.size() - p : q - p)};
    return out.empty() ? std::string{"/"} : out;
}

}

void init()     {}
void shutdown() {}

CrawlReport run(const std::vector<std::string> & seeds,
                const Fetcher & fetch,
                const CrawlConfig & cfg) {
    CrawlReport rep;
    std::unordered_set<std::string> seen_url, seen_content;
    std::unordered_set<std::string> seed_hosts;
    std::unordered_map<std::string, robots_sitemap::RobotsRules> robots_cache;
    crawl_politeness::reset();

    struct QItem { std::string url; int depth; };
    std::deque<QItem> q;
    for (const auto & s : seeds) {
        std::string canon = dedup_ingest::canonicalize_url(s);
        if (seen_url.insert(canon).second) {
            q.push_back({canon, 0});
            seed_hosts.insert(host_of(canon));
        }
    }

    auto load_robots = [&](const std::string & host) -> const robots_sitemap::RobotsRules * {
        auto it = robots_cache.find(host);
        if (it != robots_cache.end()) return &it->second;
        if (!cfg.obey_robots) {
            robots_cache[host] = robots_sitemap::RobotsRules{};
            return &robots_cache[host];
        }
        std::string scheme = "https";
        // Guess the scheme by looking at any queued URL on this host.
        for (const auto & qi : q) {
            if (host_of(qi.url) == host) { scheme = scheme_of(qi.url); break; }
        }
        std::string url = scheme + "://" + host + "/robots.txt";
        FetchResult fr = fetch(url);
        robots_sitemap::RobotsRules rules;
        if (fr.status >= 200 && fr.status < 300) rules = robots_sitemap::parse_robots(fr.body);
        robots_cache[host] = rules;
        return &robots_cache[host];
    };

    while (!q.empty() && rep.visited.size() < cfg.max_pages) {
        QItem cur = q.front(); q.pop_front();
        rep.depth_by_url[cur.url] = cur.depth;

        std::string host = host_of(cur.url);
        if (host.empty()) continue;

        if (cfg.obey_robots) {
            const auto * rules = load_robots(host);
            if (rules && !robots_sitemap::is_allowed(*rules, cfg.user_agent,
                                                    path_of(cur.url))) {
                rep.blocked_robots.push_back(cur.url);
                continue;
            }
        }

        // Politeness budget: spin retry_after_seconds back as "skip" rather
        // than block. Real wall-clock waits live in the fetcher.
        auto budget = crawl_politeness::check_budget(host,
            cfg.max_per_minute_per_host, cfg.min_gap_seconds, now_unix());
        if (!budget.allowed) {
            // Put it back at the end; if it loops we'll bail via max_pages.
            q.push_back(cur);
            continue;
        }

        FetchResult fr = fetch(cur.url);
        crawl_politeness::track(host, fr.status, (int64_t)fr.body.size(),
                                /*elapsed_ms*/ 0, now_unix());
        if (fr.status >= 400) { rep.dead.push_back(cur.url); continue; }

        // Content-hash dedup: byte-exact (FNV-1a 64) so two URLs that
        // serve the IDENTICAL body collapse, but pages that differ even
        // slightly still get extracted. SimHash is too aggressive here;
        // it would conflate a navigation page with its children when
        // they share most boilerplate.
        uint64_t fp = 0xcbf29ce484222325ULL;
        for (unsigned char c : fr.body) { fp ^= c; fp *= 0x100000001b3ULL; }
        std::string fp_key = std::to_string(fp);
        if (!seen_content.insert(fp_key).second) {
            // Already saw this content; still count visit (it was fetched)
            // but do not extract / expand.
            rep.visited.push_back(cur.url);
            continue;
        }

        if (cfg.triage_drop) {
            auto art   = readability::extract(fr.body);
            auto verd  = page_triage::triage(fr.body, art.body_text,
                                              fr.status, cfg.quality_floor);
            if (verd.drop) { rep.dropped.push_back(cur.url); continue; }
        }
        rep.visited.push_back(cur.url);

        if (cur.depth >= cfg.max_depth) continue;
        auto links = link_graph::extract_links(fr.body, cur.url);
        for (const auto & l : links) {
            if (l.absolute.empty()) continue;
            std::string canon = dedup_ingest::canonicalize_url(l.absolute);
            if (!seen_url.insert(canon).second) continue;
            if (cfg.stay_on_seed_hosts && !seed_hosts.count(host_of(canon))) continue;
            q.push_back({canon, cur.depth + 1});
        }
    }
    return rep;
}

}
