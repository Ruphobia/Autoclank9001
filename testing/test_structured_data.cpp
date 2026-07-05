// Smoke test for 043_structured_data.

#include "test_runner.hpp"
#include "../modules/043_structured_data/structured_data.hpp"

namespace {

const char * kPage = R"HTML(
<html>
<head>
<meta property="og:title" content="OG title"/>
<meta content="OG description swapped attr order" property="og:description"/>
<meta property="article:author" content="Sam Lee"/>
<script type="application/ld+json">
{
  "@context": "https://schema.org",
  "@type": "Article",
  "headline": "JSON-LD headline",
  "datePublished": "2026-06-29",
  "author": {"@type":"Person","name":"Jane Bauer"}
}
</script>
<script type="application/ld+json">[
  { "@type": "Organization", "name": "ACME" }
]</script>
</head>
<body>
<div itemscope itemtype="https://schema.org/Article">
  <h1 itemprop="headline">Microdata headline</h1>
  <meta itemprop="datePublished" content="2026-06-29"/>
</div>
</body>
</html>
)HTML";

testing::TestOutcome run() {
    structured_data::init();
    auto e = structured_data::extract(kPage);

    if (e.json_ld.size() != 2) return testing::fail("json-ld block count != 2");
    bool found_article = false, found_org = false;
    for (const auto & d : e.json_ld) {
        if (d.value("@type", "") == "Article")     found_article = true;
        if (d.value("@type", "") == "Organization") found_org    = true;
    }
    if (!found_article || !found_org) return testing::fail("json-ld types missed");

    if (e.opengraph["og:title"] != "OG title")
        return testing::fail("og:title parse wrong");
    if (e.opengraph["og:description"].find("swapped attr order") == std::string::npos)
        return testing::fail("og:description swapped-attr parse wrong");
    if (e.opengraph["article:author"] != "Sam Lee")
        return testing::fail("article:author parse wrong");

    if (e.microdata["headline"] != "Microdata headline" &&
        e.microdata["headline"].empty())
        return testing::fail("microdata text not captured");
    if (e.microdata["datePublished"] != "2026-06-29")
        return testing::fail("microdata content-attr not captured");

    auto m = structured_data::merge(e);
    if (m["headline"].empty()) return testing::fail("merge lost headline");
    if (m["og:title"] != "OG title") return testing::fail("merge dropped og:title");

    structured_data::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "structured_data",
    "043_structured_data: JSON-LD + OpenGraph + microdata extraction + merge",
    &run);

}
