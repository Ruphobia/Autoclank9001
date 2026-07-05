#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

// Structured Data Scraper: pulls JSON-LD, OpenGraph meta tags, and
// basic itemprop microdata from an HTML body and reconciles them
// into a schema.org-shaped record per URL. RDFa is intentionally
// skipped at this layer; it covers very few real pages.
namespace structured_data {

struct Extraction {
    std::vector<nlohmann::json>     json_ld;       // every parsed JSON-LD block
    std::map<std::string, std::string> opengraph;  // og: + article: + twitter: tags
    std::map<std::string, std::string> microdata;   // itemprop=name -> first text seen
};

void init();
void shutdown();

// Pure parser; no networking. Caller hands the page body and gets the
// three signal lists back. Reconciliation into a single record is the
// caller's job (preference order varies by use case).
Extraction extract(std::string_view html);

// Convenience: merge the three sources into a single key->value map.
// Precedence is JSON-LD top-level scalars first, then OpenGraph, then
// microdata. For multi-valued JSON-LD fields the first scalar wins.
std::map<std::string, std::string> merge(const Extraction & e);

}
