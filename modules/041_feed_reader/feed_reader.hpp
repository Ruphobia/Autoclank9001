#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Feed Reader: parses RSS 2.0, Atom 1.0, and JSON Feed 1.x bodies
// into a common record shape. No networking; the crawler / fetcher
// layer hands a body and content-type and gets a Feed back.
namespace feed_reader {

enum class FeedKind { Unknown = 0, Rss = 1, Atom = 2, JsonFeed = 3 };
const char * to_string(FeedKind);

struct Entry {
    std::string id;             // guid / id / link as a stable identifier
    std::string title;
    std::string link;
    std::string published;      // raw timestamp string as the feed gives it
    std::string summary;
};

struct Feed {
    FeedKind            kind = FeedKind::Unknown;
    std::string         title;
    std::string         link;
    std::string         description;
    std::vector<Entry>  entries;
};

void init();
void shutdown();

// Heuristic content-type / sniffing dispatch. content_type may be
// empty; the body is sniffed for a leading "{" or "<rss" / "<feed".
Feed parse(std::string_view body, std::string_view content_type = {});

// Format-specific entry points; useful when you already know the
// flavor (or want to bypass the sniffer in tests).
Feed parse_rss     (std::string_view body);
Feed parse_atom    (std::string_view body);
Feed parse_json_feed(std::string_view body);

}
