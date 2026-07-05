#include "robots_sitemap.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace robots_sitemap {
namespace {

std::string trim(std::string_view in) {
    size_t a = 0, b = in.size();
    while (a < b && std::isspace(static_cast<unsigned char>(in[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(in[b - 1]))) --b;
    return std::string(in.substr(a, b - a));
}

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

bool path_matches(std::string_view pattern, std::string_view path) {
    if (pattern.empty()) return false;
    // robots.txt semantics: "*" matches any (possibly empty) sequence;
    // "$" at end-of-pattern anchors end-of-path; otherwise the pattern
    // matches any URL whose path STARTS WITH the pattern (prefix match).
    bool end_anchor = pattern.back() == '$';
    bool has_star   = pattern.find('*') != std::string_view::npos;
    if (!has_star && !end_anchor) {
        if (path.size() < pattern.size()) return false;
        return path.compare(0, pattern.size(), pattern) == 0;
    }
    if (end_anchor) pattern.remove_suffix(1);

    // Wildcard matcher: '*' matches any (possibly empty) substring; all
    // other characters match literally. With end_anchor the pattern
    // must consume the entire path; without it the pattern only has to
    // match a prefix.
    size_t pi = 0, si = 0;
    size_t star_p = std::string_view::npos, star_s = 0;
    while (si < path.size()) {
        if (pi < pattern.size() && pattern[pi] == '*') {
            star_p = pi++;
            star_s = si;
        } else if (pi < pattern.size() && pattern[pi] == path[si]) {
            ++pi; ++si;
        } else if (star_p != std::string_view::npos) {
            pi = star_p + 1;
            si = ++star_s;
        } else {
            return false;
        }
        if (pi == pattern.size() && !end_anchor) return true;
    }
    while (pi < pattern.size() && pattern[pi] == '*') ++pi;
    if (pi < pattern.size()) return false;
    if (end_anchor && si != path.size()) return false;
    return true;
}

size_t effective_len(std::string_view pattern) {
    size_t n = 0;
    for (char c : pattern) if (c != '*' && c != '$') ++n;
    return n;
}

// Find rules block for the user agent; falls back to "*" if a UA
// match has no own block.
const RobotsGroup * pick_group(const RobotsRules & r, std::string_view ua) {
    std::string lua = lower(ua);
    const RobotsGroup * best = nullptr;
    size_t best_len = 0;
    for (const auto & g : r.groups) {
        for (const auto & u : g.user_agents) {
            std::string lu = lower(u);
            if (lu == "*") {
                if (!best && best_len == 0) best = &g;
                continue;
            }
            if (lua.find(lu) != std::string::npos && lu.size() > best_len) {
                best = &g;
                best_len = lu.size();
            }
        }
    }
    return best;
}

bool starts_with_ci(const std::string & line, const std::string & prefix) {
    if (line.size() < prefix.size()) return false;
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower((unsigned char)line[i]) != std::tolower((unsigned char)prefix[i])) return false;
    }
    return true;
}

std::string after_colon(const std::string & line) {
    auto pos = line.find(':');
    if (pos == std::string::npos) return {};
    return trim(line.substr(pos + 1));
}

std::string between(const std::string & body, std::string::size_type from,
                    const std::string & open_tag, const std::string & close_tag,
                    std::string::size_type & cursor_out) {
    auto a = body.find(open_tag, from);
    if (a == std::string::npos) { cursor_out = std::string::npos; return {}; }
    a += open_tag.size();
    auto b = body.find(close_tag, a);
    if (b == std::string::npos) { cursor_out = std::string::npos; return {}; }
    cursor_out = b + close_tag.size();
    return body.substr(a, b - a);
}

std::string field_in_url_block(const std::string & block, const std::string & name) {
    std::string open  = "<" + name + ">";
    std::string close = "</" + name + ">";
    auto a = block.find(open);
    if (a == std::string::npos) return {};
    a += open.size();
    auto b = block.find(close, a);
    if (b == std::string::npos) return {};
    return trim(block.substr(a, b - a));
}

}

void init()     {}
void shutdown() {}

RobotsRules parse_robots(std::string_view body) {
    RobotsRules r;
    RobotsGroup cur;
    bool        in_group = false;
    bool        last_was_ua = false;

    std::istringstream is{std::string(body)};
    std::string line;
    while (std::getline(is, line)) {
        // strip comments
        auto hash = line.find('#');
        if (hash != std::string::npos) line.erase(hash);
        std::string t = trim(line);
        if (t.empty()) continue;

        if (starts_with_ci(t, "user-agent:")) {
            std::string val = after_colon(t);
            if (in_group && !last_was_ua) {
                // Starting a new group; flush current.
                r.groups.push_back(std::move(cur));
                cur = RobotsGroup{};
            }
            cur.user_agents.push_back(val);
            in_group = true;
            last_was_ua = true;
            continue;
        }
        last_was_ua = false;

        if (starts_with_ci(t, "disallow:")) {
            std::string val = after_colon(t);
            // Empty disallow means "nothing disallowed"; skip.
            if (!val.empty()) cur.disallow.push_back(val);
        } else if (starts_with_ci(t, "allow:")) {
            std::string val = after_colon(t);
            if (!val.empty()) cur.allow.push_back(val);
        } else if (starts_with_ci(t, "crawl-delay:")) {
            try { cur.crawl_delay_seconds = std::stoi(after_colon(t)); }
            catch (...) {}
        } else if (starts_with_ci(t, "sitemap:")) {
            r.sitemaps.push_back(after_colon(t));
        } else if (starts_with_ci(t, "host:")) {
            r.host_hints.push_back(after_colon(t));
        }
    }
    if (in_group) r.groups.push_back(std::move(cur));
    return r;
}

bool is_allowed(const RobotsRules & rules, std::string_view ua, std::string_view path) {
    const auto * g = pick_group(rules, ua);
    if (!g) return true;

    size_t best_allow_len    = 0;
    size_t best_disallow_len = 0;
    bool   any_allow_hit     = false;
    bool   any_disallow_hit  = false;
    for (const auto & p : g->allow)    if (path_matches(p, path)) {
        any_allow_hit = true;
        best_allow_len = std::max(best_allow_len, effective_len(p));
    }
    for (const auto & p : g->disallow) if (path_matches(p, path)) {
        any_disallow_hit = true;
        best_disallow_len = std::max(best_disallow_len, effective_len(p));
    }
    if (!any_disallow_hit) return true;
    if (!any_allow_hit)    return false;
    return best_allow_len >= best_disallow_len;
}

int crawl_delay_for(const RobotsRules & r, std::string_view ua) {
    const auto * g = pick_group(r, ua);
    return g ? g->crawl_delay_seconds : 0;
}

std::vector<SitemapUrl> parse_sitemap_xml(std::string_view body_v) {
    std::string body{body_v};
    std::vector<SitemapUrl> out;
    std::string::size_type cursor = 0;
    while (cursor != std::string::npos) {
        auto next = body.find("<url>", cursor);
        if (next == std::string::npos) break;
        std::string::size_type past = 0;
        std::string block = between(body, next, "<url>", "</url>", past);
        if (past == std::string::npos) break;
        SitemapUrl u;
        u.loc        = field_in_url_block(block, "loc");
        u.lastmod    = field_in_url_block(block, "lastmod");
        u.changefreq = field_in_url_block(block, "changefreq");
        u.priority   = field_in_url_block(block, "priority");
        if (!u.loc.empty()) out.push_back(u);
        cursor = past;
    }
    return out;
}

std::vector<std::string> parse_sitemap_index_xml(std::string_view body_v) {
    std::string body{body_v};
    std::vector<std::string> out;
    std::string::size_type cursor = 0;
    while (cursor != std::string::npos) {
        auto next = body.find("<sitemap>", cursor);
        if (next == std::string::npos) break;
        std::string::size_type past = 0;
        std::string block = between(body, next, "<sitemap>", "</sitemap>", past);
        if (past == std::string::npos) break;
        std::string loc = field_in_url_block(block, "loc");
        if (!loc.empty()) out.push_back(loc);
        cursor = past;
    }
    return out;
}

}
