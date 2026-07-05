// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 049_page_triage.

#include "test_runner.hpp"
#include "../modules/049_page_triage/page_triage.hpp"

namespace {

testing::TestOutcome run() {
    page_triage::init();

    // Language detection.
    if (page_triage::detect_language(
            "The quick brown fox jumps over the lazy dog and that is a fact for this test."
        ) != "en")
        return testing::fail("en not detected");
    if (page_triage::detect_language(
            "El gato y el perro estan en la casa de la abuela para una fiesta del verano."
        ) != "es")
        return testing::fail("es not detected");
    if (page_triage::detect_language("???") != "?")
        return testing::fail("unknown should be '?'");

    // Paywall.
    if (!page_triage::detect_paywall("<div class=paywall></div>",
            "Subscribe to continue reading this article."))
        return testing::fail("paywall not detected");
    if (page_triage::detect_paywall("", "an open article body"))
        return testing::fail("paywall false positive");

    // Error page.
    if (!page_triage::detect_error_page("", 404)) return testing::fail("404 not detected");
    if (!page_triage::detect_error_page("This page does not exist on the server."))
        return testing::fail("404 text not detected");
    if (page_triage::detect_error_page("All good here.", 200))
        return testing::fail("good page flagged as error");

    // Topic tags.
    auto sci = page_triage::topic_tags(
        "Abstract: we present a method. DOI: 10.1/abc. Methods: ... Conclusion: ...");
    bool found = false;
    for (const auto & t : sci) if (t == "science") found = true;
    if (!found) return testing::fail("science topic missed");

    auto cod = page_triage::topic_tags(
        "function foo() { return 1; } const x = 2;");
    bool found_code = false;
    for (const auto & t : cod) if (t == "code") found_code = true;
    if (!found_code) return testing::fail("code topic missed");

    // Quality bucket: short -> low; long literate -> higher.
    double q_short = page_triage::estimate_quality("Hi.");
    if (q_short > 0.2) return testing::fail("short text too high quality");

    std::string essay;
    for (int i = 0; i < 80; ++i)
        essay += "This is a clear sentence about science. ";
    double q_long = page_triage::estimate_quality(essay);
    if (q_long < 0.4)
        return testing::fail("essay too low quality: " + std::to_string(q_long));

    // Full triage drops the paywalled / error / low-quality cases and
    // keeps the essay.
    auto t_pay = page_triage::triage("<paywall>",
        "Subscribe to continue reading", 200);
    if (!t_pay.drop) return testing::fail("paywall not dropped");

    auto t_err = page_triage::triage("", "any body", 500);
    if (!t_err.drop) return testing::fail("http 500 not dropped");

    auto t_ok = page_triage::triage("", essay, 200);
    if (t_ok.drop) return testing::fail("essay dropped");

    page_triage::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "page_triage",
    "049_page_triage: language + paywall + error page + topic tags + quality + drop",
    &run);

}
