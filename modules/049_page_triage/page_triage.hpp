#pragma once

#include <string>
#include <string_view>
#include <vector>

// AI Page Triage: heuristic classifier that filters junk out of the
// crawl queue before extraction touches it. No model load at this
// layer; an optional LLM pass (skipped for the smoke test) can layer
// on top of triage() to refine the topic_tags and quality estimate.
namespace page_triage {

struct TriageReport {
    std::string              language;          // best-guess "en" / "es" / ... / "?"
    double                   quality      = 0.0; // 0..1; higher is better
    bool                     paywall      = false;
    bool                     error_page   = false;
    std::vector<std::string> topic_tags;        // e.g. {"news","ad","code"}
    bool                     drop         = false; // recommended drop verdict
    std::string              reason;             // why drop=true, when applicable
};

void init();
void shutdown();

// Cheap language guess by frequency of high-signal common words.
// Returns "?" when no clear leader emerges. Supported: en, es, fr, de.
std::string detect_language(std::string_view text);

// 0..1 quality score from length, sentence count, alpha ratio, link
// density (when html is supplied), and ad/boilerplate keyword
// frequency. Tuned to flag near-empty or boilerplate-dominant pages.
double estimate_quality(std::string_view text, std::string_view html = {});

bool detect_paywall(std::string_view html, std::string_view text);
bool detect_error_page(std::string_view text, int http_status = 0);

std::vector<std::string> topic_tags(std::string_view text);

// Compose the full report.
TriageReport triage(std::string_view html, std::string_view extracted_text,
                    int http_status = 0,
                    double quality_floor = 0.25);

}
