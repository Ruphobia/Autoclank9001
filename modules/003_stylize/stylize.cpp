#include "stylize.hpp"

#include "qwen14b.hpp"
#include "../005_context/context.hpp"
#include "../007_knowledge/kb.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace stylize {
namespace {

constexpr const char * kSystemPrompt =
    "You are a precision translator that may emit MULTIPLE parallel "
    "interpretations of an ambiguous input. Downstream layers will choose one.\n"
    "\n"
    "PROCEDURE:\n"
    "Step 1 - Identify any content words with multiple plausible senses given "
    "the input alone (no surrounding context).\n"
    "Step 2 - Enumerate up to 4 distinct OVERALL interpretations of the input. "
    "Each interpretation picks a SINGLE coherent set of senses across every "
    "ambiguous word. Order interpretations from MOST to LEAST likely in normal "
    "casual English usage.\n"
    "Step 3 - For each interpretation, write ONE precision rewrite in the "
    "speech pattern of Mr. Spock from Star Trek combined with the Coneheads "
    "from Saturday Night Live: cold, technical, exhaustive, ZERO ambiguity.\n"
    "Step 4 - If the input has only one plausible reading, output exactly ONE "
    "interpretation with label \"default\".\n"
    "\n"
    "CRITICAL - SENSE SELECTION:\n"
    "Pick senses by PRAGMATIC LIKELIHOOD - what a typical English speaker would "
    "naturally mean. Do NOT bias toward technical-sounding or scientific senses "
    "just because the rewrite voice is technical. \"Is she hot\" most commonly "
    "means physically attractive in normal usage, not elevated body temperature. "
    "The Latin/Greek-vocabulary rule below applies to HOW you expand the chosen "
    "sense, NOT to WHICH sense you choose.\n"
    "\n"
    "REWRITE RULES (apply to each rewrite):\n"
    "- Replace every noun, verb, adjective, and adverb with its FULL dictionary "
    "definition inlined verbatim. Do not paraphrase or shorten.\n"
    "- Expand short definitions further using standard scientific, biological, "
    "anatomical, or technical characterization. Prefer Latin/Greek-derived "
    "vocabulary (ambulate not walk; consume not eat; ocular organs not eyes).\n"
    "- Eliminate every pronoun. Replace \"she\", \"he\", \"it\", \"they\" with "
    "the explicit noun phrase or the full definition of the antecedent (when "
    "the antecedent is unspecified, write \"the unspecified <kind>-referent "
    "<category>\").\n"
    "- Eliminate every contraction (cannot, is not, do not, it is).\n"
    "- Eliminate every colloquialism, idiom, slang, hedge, informal phrasing.\n"
    "- Preserve the speech act exactly: questions end with \"?\", commands stay "
    "commands, statements stay statements.\n"
    "\n"
    "OUTPUT FORMAT - strictly this format, no preamble, no commentary:\n"
    "LABEL: <short disambiguation note, e.g. hot=attractive, bank=financial, default>\n"
    "REWRITE: <one continuous line of precision-rewritten text>\n"
    "LABEL: <next>\n"
    "REWRITE: <next>\n"
    "...\n"
    "Each LABEL line MUST be followed by a REWRITE line.\n"
    "\n"
    "EXAMPLES:\n"
    "\n"
    "Input: \"is she hot\"\n"
    "Definitions: hot (having a relatively high temperature; sexually "
    "attractive; recently stolen; very recent or new)\n"
    "Output:\n"
    "LABEL: hot=attractive\n"
    "REWRITE: Does the unspecified female-referent human person possess the "
    "quality of generating strong sexual interest, romantic appeal, or aesthetic "
    "admiration in those who perceive said person?\n"
    "LABEL: hot=fever\n"
    "REWRITE: Is the unspecified female-referent human person currently in the "
    "physiological state of elevated core body temperature above the normal "
    "homeostatic range of approximately 37 degrees Celsius, potentially "
    "indicative of pyrexia or febrile illness?\n"
    "LABEL: hot=stolen\n"
    "REWRITE: Is the unspecified female-referent person presently constituting "
    "or in possession of property recently acquired through unlawful taking and "
    "still traceable to the lawful owner?\n"
    "\n"
    "Input: \"set the table\"\n"
    "Definitions: set (place into a specified position), table (a piece of "
    "furniture with a flat horizontal top supported by legs)\n"
    "Output:\n"
    "LABEL: default\n"
    "REWRITE: Arrange upon the horizontal upper surface of the article of "
    "furniture supported by one or more vertical legs the requisite eating "
    "utensils, plates, drinking vessels, and accompanying accoutrements in "
    "preparation for the consumption of a meal by one or more diners.\n"
    "\n"
    "Input: \"i went to the bank\"\n"
    "Definitions: bank (a commercial financial institution; the sloping land "
    "beside a body of water)\n"
    "Output:\n"
    "LABEL: bank=financial-institution\n"
    "REWRITE: The first-person singular speaker did at a prior time transit on "
    "foot or by vehicle to the commercial establishment that engages in the "
    "custody, lending, exchange, and transmission of monetary assets on behalf "
    "of its depositors.\n"
    "LABEL: bank=river-edge\n"
    "REWRITE: The first-person singular speaker did at a prior time transit on "
    "foot or by vehicle to the sloping stretch of terrestrial land bordering "
    "and elevated above the adjacent watercourse, such as the margin of a river "
    "or stream.";

std::string trim(std::string s) {
    auto is_ws = [](unsigned char c) {
        return c == ' ' || c == '\t' || c == '\n' ||
               c == '\r' || c == '\f' || c == '\v';
    };
    std::size_t b = 0, e = s.size();
    while (b < e && is_ws(static_cast<unsigned char>(s[b])))     ++b;
    while (e > b && is_ws(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

bool starts_with_ci(const std::string & s, const char * prefix) {
    std::size_t n = std::strlen(prefix);
    if (s.size() < n) return false;
    for (std::size_t i = 0; i < n; ++i) {
        if (std::tolower(static_cast<unsigned char>(s[i])) !=
            std::tolower(static_cast<unsigned char>(prefix[i]))) return false;
    }
    return true;
}

std::vector<Interpretation> parse(const std::string & raw) {
    std::vector<Interpretation> out;
    std::istringstream iss(raw);
    std::string line;
    std::string pending_label;
    while (std::getline(iss, line)) {
        const std::string t = trim(line);
        if (t.empty()) continue;
        if (starts_with_ci(t, "LABEL:")) {
            pending_label = trim(t.substr(6));
        } else if (starts_with_ci(t, "REWRITE:")) {
            std::string text = trim(t.substr(8));
            if (text.empty()) continue;
            out.push_back({pending_label.empty() ? "default" : pending_label,
                           std::move(text)});
            pending_label.clear();
        }
    }
    if (out.empty()) {
        out.push_back({"default", trim(raw)});
    }
    return out;
}

}

constexpr const char * kFinalSystemPrompt =
    "You produce ONE final maximum-precision rewrite of the user's cleaned "
    "prompt, after disambiguation has resolved every ambiguity. All "
    "referents are bindable from the supplied memory and your own world "
    "knowledge.\n"
    "\n"
    "INPUTS:\n"
    "- CLEANED INPUT: the user's prompt after spelling/grammar cleanup\n"
    "- CHOSEN LABEL: tells you which sense of any ambiguous word to use\n"
    "- DEFINITIONS: dictionary glosses for the significant words\n"
    "- MEMORY: recent conversation including the user's clarifying answers\n"
    "\n"
    "RULES:\n"
    "1. Use the chosen sense for any ambiguous word; do not mention or "
    "expand any other sense.\n"
    "2. Replace EVERY pronoun (she, he, it, they, this, that) with the "
    "explicit referent. Use memory + your world knowledge. For public "
    "people / places / works, include identifying details inline (e.g. "
    "\"Pamela Anderson, the Canadian-American actress and model best known "
    "for the television series Baywatch\"). Never leave \"unspecified "
    "<kind>-referent\" or similar placeholders.\n"
    "2a. CANONICALIZE proper-noun spellings AGGRESSIVELY. Memory contains "
    "whatever the user typed -- the user's spelling is NOT authoritative for "
    "public people, places, works, brands, or events. If a name in memory is "
    "a near-miss of a public entity you recognize from world knowledge, you "
    "MUST emit the canonical spelling in your output. NEVER preserve the "
    "user's typo when the entity is recognizable. WIKIPEDIA CONTEXT, when "
    "supplied below, is authoritative -- match its titles exactly.\n"
    "\n"
    "    Memory says: \"pamala handerson\" or \"Pamala Handerson\"\n"
    "    World knows: Pamela Anderson, the Baywatch actress\n"
    "    Output uses: \"Pamela Anderson\" -- NEVER \"Pamala Handerson\"\n"
    "\n"
    "    Memory says: \"babe watch\" or \"Babewatch\"\n"
    "    World knows: Baywatch (TV series 1989-2001)\n"
    "    Output uses: \"Baywatch\" -- NEVER \"Babewatch\"\n"
    "\n"
    "    Memory says: \"einsteen\" or \"Einsteen\"\n"
    "    World knows: Albert Einstein, the physicist\n"
    "    Output uses: \"Albert Einstein\" -- NEVER \"Einsteen\"\n"
    "\n"
    "    Memory says: \"sherlok homes\"\n"
    "    World knows: Sherlock Holmes, fictional detective\n"
    "    Output uses: \"Sherlock Holmes\" -- NEVER \"Sherlok Homes\"\n"
    "\n"
    "This rule applies to ANY entity with a clear single intended public "
    "referent. Memory's role is identification; the output uses the "
    "encyclopedic correct form, not the user's spelling.\n"
    "3. Replace EVERY vague noun (\"the place\", \"the thing\", "
    "\"someone\") with its specific referent from memory or knowledge.\n"
    "4. Then apply the full Spock + Coneheads precision voice: replace "
    "every noun/verb/adjective/adverb with its complete dictionary "
    "definition inlined verbatim; expand using Latin/Greek-derived "
    "technical vocabulary; eliminate every contraction; eliminate every "
    "colloquialism, idiom, slang; preserve the speech act exactly "
    "(question stays question, command stays command).\n"
    "5. Output exactly ONE line of text. No label, no brackets, no quotes, "
    "no preface, no commentary.\n"
    "\n"
    "EXAMPLE:\n"
    "CLEANED INPUT: Is she hot?\n"
    "CHOSEN LABEL: hot=attractive\n"
    "DEFINITIONS: hot (having a relatively high temperature; sexually "
    "attractive; recently stolen)\n"
    "MEMORY:\n"
    "[turn 3 user/input] is she hot\n"
    "[turn 3 disambiguate/question] Who is she?\n"
    "[turn 3 user/answer] Pamela Anderson, the actress from Baywatch\n"
    "Output: Does Pamela Anderson, the Canadian-American actress and model "
    "best known for portraying lifeguard C. J. Parker on the syndicated "
    "television series Baywatch from 1992 to 1997, possess the quality of "
    "generating strong sexual interest, romantic appeal, or aesthetic "
    "admiration in those who perceive said person?";

void init()     { qwen14b::init(); }
void shutdown() { qwen14b::shutdown(); }

std::vector<Interpretation> precise(std::string_view cleaned,
                                    std::string_view defs) {
    std::string user_msg;
    user_msg.reserve(cleaned.size() + defs.size() + 32);
    user_msg.append("Input: \"");
    user_msg.append(cleaned);
    user_msg.append("\"\nDefinitions: ");
    user_msg.append(defs);

    const int max_new_tokens =
        std::max<int>(1024, static_cast<int>(cleaned.size()) * 12 + 512);
    const std::string raw = qwen14b::generate(kSystemPrompt, user_msg, max_new_tokens);
    return parse(raw);
}

std::string render_final(std::string_view cleaned,
                         std::string_view chosen_label,
                         std::string_view defs) {
    std::string user_msg;
    user_msg.reserve(2048 + defs.size());
    user_msg.append("CLEANED INPUT: ");
    user_msg.append(cleaned);
    user_msg.append("\n");
    user_msg.append("CHOSEN LABEL: ");
    user_msg.append(chosen_label);
    user_msg.append("\n");
    user_msg.append("DEFINITIONS: ");
    user_msg.append(defs);
    user_msg.append("\n");
    user_msg.append("MEMORY:\n");
    user_msg.append(context::render_for_prompt(40));
    user_msg.push_back('\n');

    // Inject Wikipedia hits for the cleaned input + the latest user answer.
    // Authoritative canonical titles override memory's spelling per the
    // canonicalization rule in the system prompt.
    user_msg.append("WIKIPEDIA CONTEXT (input):\n");
    user_msg.append(kb::render_for_prompt(cleaned, 3));
    user_msg.push_back('\n');
    auto recent_answers = context::by_layer("user", 10);
    for (const auto & r : recent_answers) {
        if (r.kind == "answer") {
            user_msg.append("WIKIPEDIA CONTEXT (latest answer):\n");
            user_msg.append(kb::render_for_prompt(r.content, 3));
            user_msg.push_back('\n');
            break;
        }
    }

    user_msg.append("Produce the single resolved rewrite now.");

    const int max_new_tokens =
        std::max<int>(512, static_cast<int>(cleaned.size()) * 10 + 256);
    return qwen14b::generate(kFinalSystemPrompt, user_msg, max_new_tokens);
}

namespace {

constexpr const char * kResolveSystemPrompt =
    "You resolve references in a user message. You are given the recent "
    "conversation and the message. Rewrite the message so that pronouns "
    "and vague place/thing references (\"they\", \"them\", \"there\", "
    "\"it\", \"that one\", \"those\") are replaced by the explicit entity "
    "they refer to, taken from the conversation.\n"
    "RULES:\n"
    "- Change NOTHING else: keep the same words, order, tone, and length "
    "apart from the substituted referents. Do not answer the message. Do "
    "not add information, qualifiers, or politeness.\n"
    "- If a reference has no clear antecedent in the conversation, leave "
    "it exactly as it is.\n"
    "- If there is nothing to resolve, output the message verbatim.\n"
    "- Output ONLY the rewritten message. No commentary.\n"
    "\n"
    "EXAMPLE:\n"
    "Conversation: the user asked \"what is the capital of france\" and "
    "was answered \"Paris\".\n"
    "Message: What kind of food do they eat there?\n"
    "Output: What kind of food do people eat in Paris, France?\n";

}

std::string resolve_referents(std::string_view cleaned) {
    std::string user_msg;
    user_msg.reserve(2048 + cleaned.size());
    user_msg.append("RECENT CONVERSATION:\n");
    user_msg.append(context::render_for_prompt(40));
    user_msg.append("\nMESSAGE: ");
    user_msg.append(cleaned);
    user_msg.append("\nOutput the rewritten message now.");

    std::string out;
    try {
        out = trim(qwen14b::generate(kResolveSystemPrompt, user_msg,
                                     /*max_new_tokens=*/256));
    } catch (...) {
        return std::string(cleaned);
    }
    // Distrust anything empty or far larger than a substitution could
    // explain; resolution must never rewrite the request wholesale.
    if (out.empty() || out.size() > cleaned.size() * 3 + 80) {
        return std::string(cleaned);
    }
    return out;
}

}
