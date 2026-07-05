#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

// Bibliography Generator: render a list of CSL-JSON items as a
// bibliography in one of a small built-in style set. A real CSL
// engine (with the full Zotero style repository) is the eventual
// upgrade path; this layer covers the three formats people actually
// read every day (APA-ish author-date, IEEE-ish numeric, Chicago-ish
// notes-and-bibliography).
namespace bibliography {

enum class Style { ApaLike = 0, IeeeNumeric = 1, ChicagoNotes = 2 };
const char * to_string(Style);

enum class Output { Plain = 0, Markdown = 1, Html = 2 };
const char * to_string(Output);

void init();
void shutdown();

// Render one item as a single bibliography entry (no trailing newline).
std::string format_item(const nlohmann::json & item,
                        Style style, Output out = Output::Plain,
                        int numeric_index = 0);

// Render a vector of items as a full bibliography. APA-ish sorts
// alphabetically by first-author family; numeric / notes preserve
// input order.
std::string format_bibliography(const std::vector<nlohmann::json> & items,
                                Style style, Output out = Output::Plain);

}
