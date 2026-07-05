#pragma once

#include <string>
#include <string_view>

// Readability Extractor: strips boilerplate from an HTML page and
// recovers the main article body plus the byline and publish date
// where the page exposes them via OpenGraph / article: meta tags.
// Pure heuristic; no JS execution. The Headless Browser Fetcher
// handles JS-rendered pages and feeds the post-render HTML here.
namespace readability {

struct Article {
    std::string title;
    std::string byline;
    std::string publish_date;
    std::string body_text;       // boilerplate-stripped, tag-stripped text
};

void init();
void shutdown();

Article extract(std::string_view html);

}
