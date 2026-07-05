// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace dictionary {

struct Entry {
    std::string source;      // "Webster 1913" or "WordNet"
    std::string pos;         // part of speech for WordNet; empty for Webster
    std::string definition;  // definition text
};

// Loads both dictionary JSON files from disk. Idempotent and thread-safe.
// Reads paths relative to the current working directory:
//   002_dictionary/dictionary.json   (Webster 1913, PD)
//   002_dictionary/wordnet.json      (WordNet 3.1, Princeton free + attrib)
// Throws std::runtime_error if a file cannot be opened.
void init();

// Returns every Entry for a word across all loaded sources. Case-insensitive.
// Webster entries come first (single big-string definition), followed by all
// WordNet senses in their stored order. Empty vector if not found anywhere.
std::vector<Entry> lookup(std::string_view word);

// Synonyms from the Moby Thesaurus II (Grady Ward, public domain;
// resources/dictionary/mthesaur.txt). Case-insensitive root lookup, in
// the thesaurus's stored order. `max_results` <= 0 returns the full list
// (Moby roots average ~80 synonyms). Empty when the word is not a root
// or the thesaurus file is absent (the feature degrades gracefully).
std::vector<std::string> synonyms(std::string_view word, int max_results = 12);

}
