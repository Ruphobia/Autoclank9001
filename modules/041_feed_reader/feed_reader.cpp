#include "feed_reader.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

#include <nlohmann/json.hpp>

namespace feed_reader {
namespace {

std::string trim(std::string_view in) {
    size_t a = 0, b = in.size();
    while (a < b && std::isspace(static_cast<unsigned char>(in[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(in[b - 1]))) --b;
    return std::string(in.substr(a, b - a));
}

std::string field_in(const std::string & block, const std::string & name) {
    std::string open  = "<" + name + ">";
    std::string close = "</" + name + ">";
    auto a = block.find(open);
    if (a == std::string::npos) return {};
    a += open.size();
    auto b = block.find(close, a);
    if (b == std::string::npos) return {};
    std::string raw = trim(block.substr(a, b - a));
    // strip CDATA
    if (raw.size() >= 12 && raw.compare(0, 9, "<![CDATA[") == 0 &&
        raw.compare(raw.size() - 3, 3, "]]>") == 0) {
        raw = raw.substr(9, raw.size() - 12);
    }
    return raw;
}

// For <link href="..."/> in Atom; pulls the href attribute.
std::string atom_link_href(const std::string & block) {
    static const std::regex re("<link[^>]*href=\"([^\"]+)\"[^>]*/?>");
    std::smatch m;
    if (std::regex_search(block, m, re)) return m[1].str();
    return {};
}

// Walk every <tag> ... </tag> pair, returning the inner text per occurrence.
std::vector<std::string> blocks_of(const std::string & body, const std::string & tag) {
    std::vector<std::string> out;
    std::string open  = "<" + tag + ">";
    std::string close = "</" + tag + ">";
    // Allow attributes on opening tag.
    std::string open_attr = "<" + tag;
    size_t pos = 0;
    while (pos < body.size()) {
        auto a = body.find(open_attr, pos);
        if (a == std::string::npos) break;
        // Step past the opening tag (require either '>' or ' ' next char).
        auto gt = body.find('>', a);
        if (gt == std::string::npos) break;
        if (gt > a + open_attr.size() && body[a + open_attr.size()] != ' ' &&
            body[a + open_attr.size()] != '>') {
            pos = a + open_attr.size();
            continue;     // e.g. <entrymeta...> when tag=="entry"
        }
        auto b = body.find(close, gt);
        if (b == std::string::npos) break;
        out.push_back(body.substr(gt + 1, b - gt - 1));
        pos = b + close.size();
    }
    return out;
}

}

const char * to_string(FeedKind k) {
    switch (k) {
        case FeedKind::Rss:      return "rss";
        case FeedKind::Atom:     return "atom";
        case FeedKind::JsonFeed: return "json-feed";
        case FeedKind::Unknown:  return "unknown";
    }
    return "unknown";
}

void init()     {}
void shutdown() {}

Feed parse_rss(std::string_view body_v) {
    std::string body{body_v};
    Feed f;
    f.kind = FeedKind::Rss;
    // Channel-level metadata: pull from the first <channel> block.
    auto chans = blocks_of(body, "channel");
    if (!chans.empty()) {
        f.title       = field_in(chans[0], "title");
        f.link        = field_in(chans[0], "link");
        f.description = field_in(chans[0], "description");
    }
    for (const auto & item : blocks_of(body, "item")) {
        Entry e;
        e.title     = field_in(item, "title");
        e.link      = field_in(item, "link");
        e.published = field_in(item, "pubDate");
        e.summary   = field_in(item, "description");
        e.id        = field_in(item, "guid");
        if (e.id.empty()) e.id = e.link;
        f.entries.push_back(std::move(e));
    }
    return f;
}

Feed parse_atom(std::string_view body_v) {
    std::string body{body_v};
    Feed f;
    f.kind = FeedKind::Atom;
    f.title    = field_in(body, "title");
    f.link     = atom_link_href(body);
    f.description = field_in(body, "subtitle");
    for (const auto & entry : blocks_of(body, "entry")) {
        Entry e;
        e.title     = field_in(entry, "title");
        e.link      = atom_link_href(entry);
        e.published = field_in(entry, "published");
        if (e.published.empty()) e.published = field_in(entry, "updated");
        e.summary   = field_in(entry, "summary");
        if (e.summary.empty()) e.summary = field_in(entry, "content");
        e.id        = field_in(entry, "id");
        if (e.id.empty()) e.id = e.link;
        f.entries.push_back(std::move(e));
    }
    return f;
}

Feed parse_json_feed(std::string_view body_v) {
    Feed f;
    f.kind = FeedKind::JsonFeed;
    try {
        auto j = nlohmann::json::parse(body_v);
        f.title       = j.value("title", "");
        f.link        = j.value("home_page_url", "");
        f.description = j.value("description", "");
        if (j.contains("items") && j["items"].is_array()) {
            for (const auto & it : j["items"]) {
                Entry e;
                e.id        = it.value("id", "");
                e.title     = it.value("title", "");
                e.link      = it.value("url", "");
                e.published = it.value("date_published", "");
                e.summary   = it.value("summary", "");
                if (e.summary.empty()) e.summary = it.value("content_text", "");
                f.entries.push_back(std::move(e));
            }
        }
    } catch (...) {
        f.kind = FeedKind::Unknown;
    }
    return f;
}

Feed parse(std::string_view body, std::string_view content_type) {
    std::string ct{content_type};
    std::transform(ct.begin(), ct.end(), ct.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    if (ct.find("json") != std::string::npos) return parse_json_feed(body);
    if (ct.find("atom") != std::string::npos) return parse_atom(body);
    if (ct.find("rss")  != std::string::npos) return parse_rss(body);

    // Sniff: skip leading whitespace.
    size_t i = 0;
    while (i < body.size() && std::isspace(static_cast<unsigned char>(body[i]))) ++i;
    if (i < body.size() && body[i] == '{') return parse_json_feed(body);
    if (body.find("<feed") != std::string_view::npos) return parse_atom(body);
    if (body.find("<rss")  != std::string_view::npos ||
        body.find("<channel>") != std::string_view::npos) return parse_rss(body);
    Feed f;
    return f;
}

}
