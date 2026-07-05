#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// Crawl Politeness Dashboard: in-process per-host rate accounting and
// budget enforcement. Designed for the crawler to call before each
// fetch (check_budget) and after each fetch (track), and for the UI
// to render report() periodically.
namespace crawl_politeness {

struct HostStats {
    int64_t      requests           = 0;
    int64_t      errors_4xx         = 0;
    int64_t      errors_5xx         = 0;
    int64_t      total_bytes        = 0;
    int64_t      total_latency_ms   = 0;
    int64_t      last_request_unix  = 0;
    int64_t      robots_violations  = 0;
    bool         blacklisted        = false;
    // Sliding window of request timestamps (unix seconds), kept tight
    // by the budget call.
    std::vector<int64_t> window;
};

struct BudgetCheck {
    bool    allowed             = true;
    int64_t retry_after_seconds = 0;
    std::string reason;
};

void init();
void shutdown();

// Reset all state. Useful between tests.
void reset();

// Record one fetch. now_unix=0 uses the current wall clock.
void track(std::string_view host, int status_code, int64_t bytes,
           int64_t elapsed_ms, int64_t now_unix = 0);

// Check whether one more fetch to `host` is allowed under `max_per_minute`
// and `min_gap_seconds` between requests. now_unix=0 uses the current
// wall clock. retry_after_seconds is the smallest delay that would
// satisfy the rule.
BudgetCheck check_budget(std::string_view host,
                         int  max_per_minute,
                         int  min_gap_seconds = 0,
                         int64_t now_unix = 0);

void mark_robots_violation(std::string_view host);
void mark_blacklisted(std::string_view host);
void unmark_blacklisted(std::string_view host);
bool is_blacklisted(std::string_view host);

HostStats stats(std::string_view host);
std::map<std::string, HostStats> report();

}
