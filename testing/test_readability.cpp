// Smoke test for 042_readability.

#include "test_runner.hpp"
#include "../modules/042_readability/readability.hpp"

namespace {

const char * kPage = R"HTML(
<html>
<head>
  <title>The HTML title</title>
  <meta property="og:title" content="The article OG title"/>
  <meta name="author" content="Jane Bauer"/>
  <meta property="article:published_time" content="2026-06-29T15:00:00Z"/>
</head>
<body>
  <header><nav>menu</nav></header>
  <aside>related sidebar</aside>
  <article>
    <h1>Heading</h1>
    <p>This is the first body paragraph that the extractor should return verbatim.</p>
    <p>And a second paragraph for length.</p>
    <script>doNothing();</script>
  </article>
  <footer>copyright boilerplate</footer>
</body>
</html>
)HTML";

const char * kDivPage = R"HTML(
<html><body>
<div class="ads">tiny ad</div>
<div class="main">
  <p>Long body content that should win the largest-div heuristic.</p>
  <p>Another paragraph adding more length to this region so it dominates.</p>
  <p>And a third.</p>
</div>
<div class="footer">small footer</div>
</body></html>
)HTML";

testing::TestOutcome run() {
    readability::init();
    auto a = readability::extract(kPage);
    if (a.title != "The article OG title")
        return testing::fail("og:title preference broken: '" + a.title + "'");
    if (a.byline != "Jane Bauer") return testing::fail("byline parse wrong");
    if (a.publish_date != "2026-06-29T15:00:00Z")
        return testing::fail("publish_date parse wrong");
    if (a.body_text.find("first body paragraph") == std::string::npos)
        return testing::fail("body did not retain article content");
    if (a.body_text.find("doNothing") != std::string::npos)
        return testing::fail("script body leaked into output");
    if (a.body_text.find("copyright") != std::string::npos)
        return testing::fail("footer boilerplate leaked");
    if (a.body_text.find("related sidebar") != std::string::npos)
        return testing::fail("aside boilerplate leaked");
    if (a.body_text.find("menu") != std::string::npos)
        return testing::fail("nav menu leaked");

    auto b = readability::extract(kDivPage);
    if (b.body_text.find("largest-div") == std::string::npos)
        return testing::fail("largest-div heuristic missed main region");
    if (b.body_text.find("tiny ad") != std::string::npos)
        return testing::fail("largest-div heuristic let small div through");

    readability::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "readability",
    "042_readability: meta + body extraction with boilerplate strip",
    &run);

}
