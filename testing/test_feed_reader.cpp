// Smoke test for 041_feed_reader.

#include "test_runner.hpp"
#include "../modules/041_feed_reader/feed_reader.hpp"

namespace {

const char * kRss = R"RSS(
<?xml version="1.0"?>
<rss version="2.0">
  <channel>
    <title>Example Feed</title>
    <link>https://example.com/</link>
    <description>A test feed</description>
    <item>
      <title>First post</title>
      <link>https://example.com/1</link>
      <pubDate>Mon, 29 Jun 2026 15:00:00 GMT</pubDate>
      <guid>https://example.com/1</guid>
      <description>The first body</description>
    </item>
    <item>
      <title>Second post</title>
      <link>https://example.com/2</link>
      <description>The second body</description>
    </item>
  </channel>
</rss>
)RSS";

const char * kAtom = R"ATOM(
<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
  <title>Atom Example</title>
  <link href="https://example.com/" rel="self"/>
  <subtitle>An atom feed</subtitle>
  <entry>
    <title>Atom one</title>
    <link href="https://example.com/a"/>
    <id>tag:example.com,2026:a</id>
    <updated>2026-06-29T15:00:00Z</updated>
    <summary>Atom one body</summary>
  </entry>
</feed>
)ATOM";

const char * kJsonFeed = R"JF({
  "version": "https://jsonfeed.org/version/1.1",
  "title": "JSON Feed example",
  "home_page_url": "https://example.com/",
  "description": "A test JSON feed",
  "items": [
    {
      "id": "j1",
      "title": "J one",
      "url": "https://example.com/j1",
      "date_published": "2026-06-29T15:00:00Z",
      "summary": "j one summary"
    }
  ]
})JF";

testing::TestOutcome run() {
    feed_reader::init();

    auto rss = feed_reader::parse_rss(kRss);
    if (rss.kind != feed_reader::FeedKind::Rss) return testing::fail("rss kind wrong");
    if (rss.title != "Example Feed") return testing::fail("rss title parse wrong");
    if (rss.entries.size() != 2) return testing::fail("rss item count wrong");
    if (rss.entries[0].title != "First post") return testing::fail("rss entry title wrong");
    if (rss.entries[0].published.empty())   return testing::fail("rss pubDate missing");
    if (rss.entries[1].id != "https://example.com/2")
        return testing::fail("rss fallback id from link wrong");

    auto atom = feed_reader::parse_atom(kAtom);
    if (atom.kind != feed_reader::FeedKind::Atom) return testing::fail("atom kind wrong");
    if (atom.entries.size() != 1) return testing::fail("atom entry count wrong");
    if (atom.entries[0].link != "https://example.com/a")
        return testing::fail("atom link href not extracted");
    if (atom.entries[0].published.empty())
        return testing::fail("atom updated->published fallback missed");

    auto jf = feed_reader::parse_json_feed(kJsonFeed);
    if (jf.kind != feed_reader::FeedKind::JsonFeed) return testing::fail("jsonfeed kind wrong");
    if (jf.entries.size() != 1 || jf.entries[0].id != "j1")
        return testing::fail("jsonfeed item parse wrong");

    // Sniffer dispatch on each.
    if (feed_reader::parse(kRss).kind != feed_reader::FeedKind::Rss)
        return testing::fail("sniff failed on rss");
    if (feed_reader::parse(kAtom).kind != feed_reader::FeedKind::Atom)
        return testing::fail("sniff failed on atom");
    if (feed_reader::parse(kJsonFeed).kind != feed_reader::FeedKind::JsonFeed)
        return testing::fail("sniff failed on jsonfeed");

    feed_reader::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "feed_reader",
    "041_feed_reader: RSS / Atom / JSON Feed parser + content-type sniff",
    &run);

}
