// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>

namespace answer {

void init();

// Answer a question using the 14B model + recent session memory + the
// local Wikipedia KB. Concise plain-text answer; no preface, no markdown.
//
// `kb_context`, when non-empty, is used as the WIKIPEDIA CONTEXT block
// instead of an internal KB lookup. The pipeline already queried the KB
// with the CLEANED user text; re-querying here with the precision
// rewrite finds nothing (article titles don't match dictionary glosses).
// `dictionary_defs`, when non-empty, is the pipeline's word-sense block,
// included as additional local grounding.
// `resolved_reading`, when non-empty, is the stylize render of the
// question. It is supplied as a sense-disambiguation aid only: answers
// must be phrased in plain wording, never in the render's dictionary
// gloss (the "city of primary importance and significance" parroting).
// `web_context`, when non-empty, is a formatted list of web-search hits
// (title / URL / snippet per row) obtained by the pipeline for questions
// whose answer benefits from post-cutoff or catalog data. Unlike
// `resolved_reading`, this block is intended to be READ and CITED --
// answers should draw facts from it and mention the source URL.
std::string respond(std::string_view question,
                    std::string_view kb_context = {},
                    std::string_view dictionary_defs = {},
                    std::string_view resolved_reading = {},
                    std::string_view web_context = {});

}
