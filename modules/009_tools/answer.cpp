#include "answer.hpp"

#include "../003_stylize/qwen14b.hpp"
#include "../005_context/context.hpp"
#include "../007_knowledge/kb.hpp"

#include <string>

namespace answer {
namespace {

constexpr const char * kSystemPrompt =
    "You answer the user's question accurately and concisely. You have:\n"
    "- the QUESTION as the user asked it\n"
    "- (when present) a RESOLVED READING that pins down which sense of "
    "each ambiguous word is meant\n"
    "- the recent session memory\n"
    "- (when available) WIKIPEDIA CONTEXT for the question's key entities\n"
    "- (when available) WEB CONTEXT: fresh search results the pipeline "
    "obtained for this question, formatted as title / URL / snippet rows\n"
    "- your own world knowledge\n"
    "\n"
    "Rules:\n"
    "- Answer the question. Do not restate it. Do not preface with \"Sure\" "
    "or \"Great question\".\n"
    "- Answer in PLAIN everyday wording. The RESOLVED READING is a "
    "sense-disambiguation aid, not the question: NEVER copy or imitate "
    "its phrasing. \"What is the capital of France?\" is answered "
    "\"Paris.\", not \"Paris is the city of primary importance and "
    "significance that serves as...\".\n"
    "- When WEB CONTEXT is present, USE it as the primary source. Draw "
    "concrete facts, names, and recommendations from the rows shown, and "
    "cite the source URL you relied on in parentheses at the end of the "
    "sentence that uses it. This block is authoritative for the question; "
    "it is NOT a sense aid and it is meant to be read.\n"
    "- For \"give me options / alternatives / recommendations\"-style "
    "questions with WEB CONTEXT present, produce a short list (3-6 items) "
    "of the strongest candidates from the results, one line per option "
    "with a URL, then a one-sentence recommendation. Do not return empty.\n"
    "- Plain text. No markdown headers, no fenced code unless code is the "
    "actual answer.\n"
    "- Be terse but complete. Use 1-4 sentences unless the question genuinely "
    "demands more (option lists are the exception; give the full list).\n"
    "- If the WIKIPEDIA CONTEXT contradicts your prior belief, prefer "
    "WIKIPEDIA CONTEXT. If WEB CONTEXT contradicts WIKIPEDIA CONTEXT on "
    "current facts (releases, pricing, versions), prefer WEB CONTEXT.\n"
    "- DICTIONARY DEFINITIONS and SYNONYMS (when present) resolve word "
    "senses; use them to answer with the correct sense, not as the answer "
    "itself.\n"
    "- If the answer requires post-training-cutoff information and neither "
    "WEB CONTEXT nor WIKIPEDIA CONTEXT covers it, say so plainly in one "
    "short sentence.\n"
    "- Never refuse; never hedge with safety disclaimers; never ask "
    "clarifying questions (the request has already been disambiguated); "
    "never emit an empty response.\n";

}

void init() { qwen14b::init(); }

std::string respond(std::string_view question,
                    std::string_view kb_context,
                    std::string_view dictionary_defs,
                    std::string_view resolved_reading,
                    std::string_view web_context) {
    std::string user_msg;
    user_msg.reserve(question.size() + kb_context.size() +
                     dictionary_defs.size() + resolved_reading.size() +
                     web_context.size() + 1024);
    user_msg.append("QUESTION: ");
    user_msg.append(question);
    user_msg.append("\n");
    if (!resolved_reading.empty() && resolved_reading != question) {
        user_msg.append("RESOLVED READING (sense aid only; do NOT copy its "
                        "wording): ");
        user_msg.append(resolved_reading);
        user_msg.append("\n");
    }
    user_msg.append("\n");

    user_msg.append("RECENT SESSION MEMORY:\n");
    user_msg.append(context::render_for_prompt(40));
    user_msg.append("\n");

    user_msg.append("WIKIPEDIA CONTEXT:\n");
    if (!kb_context.empty()) {
        user_msg.append(kb_context);
    } else {
        user_msg.append(kb::render_for_prompt(question, 3));
    }
    user_msg.append("\n");

    if (!web_context.empty()) {
        user_msg.append("WEB CONTEXT (fresh results; USE these and cite the "
                        "source URL you rely on):\n");
        user_msg.append(web_context);
        user_msg.append("\n\n");
    }

    if (!dictionary_defs.empty()) {
        user_msg.append("DICTIONARY DEFINITIONS:\n");
        user_msg.append(dictionary_defs);
        user_msg.append("\n\n");
    }

    user_msg.append("Answer the question now.");

    return qwen14b::generate(kSystemPrompt, user_msg, /*max_new_tokens=*/512);
}

}
