#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

// Format Import-Export Hub for reference records. Reads / writes
// BibTeX and RIS into the CSL-JSON canonical record shape used by
// 054_reference_library. Pure parser surface; no I/O.
//
// Coverage is the intersection of "fields people actually use" and
// "fields the three formats share without contortion": type, title,
// author, year, container-title, publisher, volume / issue / page,
// abstract, doi, url, isbn, citation key. Edge cases that need a real
// CSL stack (deeply nested issued.literal, complex name particles,
// multilingual fields) round-trip best through CSL-JSON directly.
namespace ref_formats {

void init();
void shutdown();

// BibTeX entry text -> vector of CSL-JSON objects (one per @-entry).
// The citation key, when present, is attached as "id" on the CSL-JSON
// document so the caller can feed it straight into 054_reference_library.
std::vector<nlohmann::json> parse_bibtex(std::string_view body);

// CSL-JSON -> a single BibTeX entry string (terminated by newline).
// The citation key is taken from item["id"] (falling back to "noid").
std::string to_bibtex(const nlohmann::json & item);
// Convenience: concatenate to_bibtex over every item.
std::string to_bibtex_many(const std::vector<nlohmann::json> & items);

// RIS body text -> vector of CSL-JSON objects.
std::vector<nlohmann::json> parse_ris(std::string_view body);

// CSL-JSON identity passthrough (with array unwrapping and a small
// validity check); useful for an import-from-file hub that picks the
// right parser by extension.
std::vector<nlohmann::json> parse_csl_json(std::string_view body);

// Sniff body content for the most likely flavor. Returns one of
// "bibtex", "ris", "csl-json", "unknown".
std::string sniff_format(std::string_view body);

}
