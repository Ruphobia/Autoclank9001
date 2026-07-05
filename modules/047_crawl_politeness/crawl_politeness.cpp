#include "crawl_politeness.hpp"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <unordered_map>

namespace crawl_politeness {
namespace {

std::mutex g_mu;
std::unordered_map<std::string, HostStats> g_stats;

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

HostStats & stats_ref(const std::string & host) { return g_stats[host]; }

void prune_window(HostStats & s, int64_t now, int64_t window_seconds) {
    auto cutoff = now - window_seconds;
    s.window.erase(std::remove_if(s.window.begin(), s.window.end(),
                                   [&](int64_t t){ return t < cutoff; }),
                    s.window.end());
}

}

void init()     {}
void shutdown() { /* keep state across calls; reset() is the explicit hook */ }
void reset() {
    std::lock_guard<std::mutex> lk(g_mu);
    g_stats.clear();
}

void track(std::string_view host_v, int status_code, int64_t bytes,
           int64_t elapsed_ms, int64_t when) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::string host{host_v};
    auto & s = stats_ref(host);
    s.requests           += 1;
    s.total_bytes        += bytes;
    s.total_latency_ms   += elapsed_ms;
    s.last_request_unix   = when ? when : now_unix();
    if (status_code >= 400 && status_code < 500) ++s.errors_4xx;
    if (status_code >= 500 && status_code < 600) ++s.errors_5xx;
    s.window.push_back(s.last_request_unix);
    prune_window(s, s.last_request_unix, 60);
}

BudgetCheck check_budget(std::string_view host_v,
                         int max_per_minute, int min_gap_seconds,
                         int64_t when) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::string host{host_v};
    BudgetCheck c;
    if (max_per_minute <= 0) max_per_minute = 1;
    int64_t now = when ? when : now_unix();
    auto & s = stats_ref(host);

    if (s.blacklisted) {
        c.allowed = false;
        c.retry_after_seconds = 0;
        c.reason = "host blacklisted";
        return c;
    }

    prune_window(s, now, 60);

    if ((int64_t)s.window.size() >= max_per_minute) {
        c.allowed = false;
        // Retry when the oldest in the window ages out.
        int64_t oldest = s.window.front();
        c.retry_after_seconds = std::max<int64_t>(1, (oldest + 60) - now);
        c.reason = "exceeded max_per_minute";
        return c;
    }
    if (min_gap_seconds > 0 && s.last_request_unix != 0 &&
        now - s.last_request_unix < min_gap_seconds) {
        c.allowed = false;
        c.retry_after_seconds = min_gap_seconds - (now - s.last_request_unix);
        c.reason = "under min_gap_seconds";
        return c;
    }
    c.allowed = true;
    return c;
}

void mark_robots_violation(std::string_view host_v) {
    std::lock_guard<std::mutex> lk(g_mu);
    auto & s = stats_ref(std::string{host_v});
    s.robots_violations += 1;
}
void mark_blacklisted(std::string_view host_v) {
    std::lock_guard<std::mutex> lk(g_mu);
    stats_ref(std::string{host_v}).blacklisted = true;
}
void unmark_blacklisted(std::string_view host_v) {
    std::lock_guard<std::mutex> lk(g_mu);
    stats_ref(std::string{host_v}).blacklisted = false;
}
bool is_blacklisted(std::string_view host_v) {
    std::lock_guard<std::mutex> lk(g_mu);
    auto it = g_stats.find(std::string{host_v});
    return it != g_stats.end() && it->second.blacklisted;
}

HostStats stats(std::string_view host_v) {
    std::lock_guard<std::mutex> lk(g_mu);
    auto it = g_stats.find(std::string{host_v});
    if (it == g_stats.end()) return HostStats{};
    return it->second;
}

std::map<std::string, HostStats> report() {
    std::lock_guard<std::mutex> lk(g_mu);
    std::map<std::string, HostStats> out;
    for (const auto & [h, s] : g_stats) out[h] = s;
    return out;
}

}
