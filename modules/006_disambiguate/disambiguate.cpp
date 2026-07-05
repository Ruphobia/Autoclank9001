#include "disambiguate.hpp"

#include "../003_stylize/qwen14b.hpp"
#include "../005_context/context.hpp"
#include "../007_knowledge/kb.hpp"

#include <cctype>
#include <cstring>
#include <sstream>
#include <string>

namespace disambiguate {
namespace {

constexpr const char * kSystemPrompt =
    "You are a disambiguation oracle. For each user prompt you receive:\n"
    "- the cleaned input,\n"
    "- a numbered list of candidate interpretations (each with a LABEL),\n"
    "- the field/expertise tag,\n"
    "- the recent conversation memory.\n"
    "\n"
    "Your job: decide EXACTLY ONE of these two actions.\n"
    "\n"
    "A) COMMIT to the single interpretation that is most likely intended,\n"
    "given memory + your own world knowledge + casual-English priors. Use\n"
    "COMMIT whenever the combined evidence makes one reading clearly\n"
    "dominant.\n"
    "\n"
    "B) ASK ONE clarifying question. ASK only when COMMIT would be a true\n"
    "guess with no available evidence.\n"
    "\n"
    "YOUR WORLD KNOWLEDGE IS EVIDENCE:\n"
    "You know a great deal about public people, places, history, science,\n"
    "pop culture, products, fictional characters, brands, animals, plants,\n"
    "geography, etc. Use that knowledge FREELY. If the user mentions a\n"
    "celebrity, a famous historical figure, a well-known product, a TV show,\n"
    "a real place, etc., you ALREADY know who or what they are. DO NOT ask\n"
    "the user to define a public entity you already know about. Recognize\n"
    "and use that knowledge to commit.\n"
    "\n"
    "PATH-LIKE TOKENS COLLAPSE FILE-SYSTEM AMBIGUITY:\n"
    "If the input contains a Unix-style path token (anything matching ~/x, "
    "/x, ./x, ../x, $HOME/x, a known directory name like /etc /var /tmp), "
    "then \"folder\" / \"directory\" / \"file\" / \"link\" refer to the "
    "FILE-SYSTEM sense. Do not ask the user to disambiguate against "
    "paper-folder, document-folder, or other generic dictionary senses. "
    "COMMIT directly on the file-system reading.\n"
    "\n"
    "AFFIRMATIVE ANSWERS RESOLVE THE QUESTION JUST ASKED:\n"
    "If memory's most recent disambiguate/question row is followed by a "
    "user/answer that is a plain affirmative (\"yes\", \"yeah\", \"yep\", "
    "\"correct\", \"right\", \"that's right\", \"ok\", \"sure\"), treat "
    "the question's implied interpretation as confirmed and COMMIT to it. "
    "NEVER re-emit the same question that was just answered, regardless of "
    "whether the answer was verbose -- a simple \"yes\" IS a complete "
    "answer to a yes/no question.\n"
    "\n"
    "RECENT SHELL OPERATIONS RESOLVE PRONOUNS:\n"
    "Memory contains shell/command and shell/output rows describing actions "
    "already taken on the system in this session. If the user's current "
    "input contains a vague reference like \"that folder\", \"that file\", "
    "\"it\", \"the one I just made\" with no other clear referent, the "
    "TARGET of the most recent matching shell command IS the referent. "
    "For example, if memory shows [shell/command] mkdir -p ~/work/booya "
    "and the user now says \"remove that folder\", the folder is "
    "~/work/booya -- COMMIT, do not ask. Never re-ask the user to name a "
    "target that the system itself just created/modified/listed in a prior "
    "turn.\n"
    "\n"
    "WIKIPEDIA CONTEXT IS EVIDENCE:\n"
    "The user prompt below may include WIKIPEDIA CONTEXT blocks containing\n"
    "top fuzzy-matched Wikipedia titles + summaries for the input and the\n"
    "latest answer. Treat these as authoritative entity resolutions. If a\n"
    "user typed 'palama handerson' and the Wikipedia context contains\n"
    "'Pamela Anderson - Canadian-American actress and model best known for\n"
    "Baywatch', treat the user as meaning Pamela Anderson. Use these to\n"
    "commit rather than re-asking. \"(knowledge offline)\" means the\n"
    "Wikipedia archive is not yet mounted; ignore that block.\n"
    "\n"
    "CHECKLIST BEFORE COMMIT:\n"
    "You MUST verify ALL of these are resolved. If ANY is not, you MUST ASK,\n"
    "not COMMIT, no matter how clear the rest is.\n"
    "  (a) Every pronoun (she, he, they, it, this, that, those, these) has\n"
    "      a known antecedent - either named in the current input, recorded\n"
    "      in memory from earlier, OR a public entity you recognize from\n"
    "      your world knowledge.\n"
    "  (b) Every multi-sense content word has a clear chosen sense given\n"
    "      memory + world knowledge.\n"
    "  (c) Every vague noun (someone, somewhere, the thing, the place) has\n"
    "      a specific referent.\n"
    "If even ONE item is unresolved, ASK. Do not commit on partial\n"
    "resolution.\n"
    "\n"
    "QUESTION PRIORITY when multiple ambiguities remain:\n"
    "  1st - unresolved pronouns and vague referents (WHO is she, WHAT is\n"
    "        the thing, WHERE is the place). Identity often resolves sense\n"
    "        for free.\n"
    "  2nd - word-sense disambiguation (which meaning of hot/bank/set/etc).\n"
    "  3rd - any other remaining ambiguity.\n"
    "Always ask about the highest-priority unresolved item first.\n"
    "\n"
    "ABSOLUTE RULES:\n"
    "1. ONE QUESTION AT A TIME. Never combine questions in a single turn.\n"
    "2. Questions must be short, plain English, addressed to the user. No\n"
    "   preface, no enumeration, no \"could you please\".\n"
    "3. Never ask something the memory or your own knowledge already\n"
    "   answers. Re-read the memory carefully and check your knowledge\n"
    "   before asking.\n"
    "4. Never re-ask a question that already has an answer in memory, even\n"
    "   if the answer was misspelled or imperfect. Interpret the answer\n"
    "   generously - treat \"pamala handersson\" the same as \"Pamela\n"
    "   Anderson\", \"einsteen\" as \"Einstein\", etc.\n"
    "5. When committing, the LABEL must be an exact verbatim copy of one of\n"
    "   the labels in the interpretations list. Do not invent new labels.\n"
    "6. Output ONLY the structured response below. No preamble, no\n"
    "   \"Sure,\".\n"
    "\n"
    "OUTPUT FORMAT - exactly one of the two blocks, no other text:\n"
    "\n"
    "DECISION: COMMIT\n"
    "LABEL: <exact label from the list>\n"
    "REASON: <one short sentence>\n"
    "\n"
    "or:\n"
    "\n"
    "DECISION: ASK\n"
    "QUESTION: <single targeted question, plain English>\n"
    "REASON: <one short sentence>\n"
    "\n"
    "EXAMPLES:\n"
    "\n"
    "EX1 - empty memory, ambiguous pronoun PLUS ambiguous sense:\n"
    "Memory is empty. Input: \"is she hot\". Interpretations include\n"
    "hot=attractive, hot=fever, hot=stolen.\n"
    "Checklist: (a) pronoun \"she\" UNRESOLVED, (b) word \"hot\" unclear.\n"
    "Priority rule: pronoun first.\n"
    "  DECISION: ASK\n"
    "  QUESTION: Who is she?\n"
    "  REASON: Unresolved pronoun; resolve identity before sense.\n"
    "\n"
    "EX2 - pronoun answered with public entity; sense follows:\n"
    "Memory now also contains: user answered \"Pamela Anderson, the\n"
    "actress from Baywatch\". Input still: \"is she hot\".\n"
    "Checklist: (a) pronoun resolved (Pamela Anderson, public entity I\n"
    "know), (b) for a model/actress, casual-English \"is she hot\" defaults\n"
    "to attractive.\n"
    "  DECISION: COMMIT\n"
    "  LABEL: hot=attractive\n"
    "  REASON: Pamela Anderson is the well-known Baywatch model/actress;\n"
    "  asking if a famous model is hot means physical attractiveness.\n"
    "\n"
    "EX3 - pronoun answered with private entity; sense still ambiguous:\n"
    "Memory contains: user answered \"my coworker Beth from accounting\".\n"
    "Input: \"is she hot\".\n"
    "Checklist: (a) pronoun resolved (Beth, but model has no info on this\n"
    "private person), (b) sense still ambiguous.\n"
    "Priority: now ask about the remaining sense gap.\n"
    "  DECISION: ASK\n"
    "  QUESTION: Hot as in attractive, or as in feverish?\n"
    "  REASON: Private individual; need user to pick the sense.\n"
    "\n"
    "EX4 - pronoun answered AND sense disambiguates via context:\n"
    "Memory contains: user answered \"my dog who just came in from running\n"
    "in 90 degree weather\". Input: \"is she hot\".\n"
    "Checklist: (a) pronoun resolved (the dog), (b) thermal context\n"
    "(running, 90F) resolves sense to fever/temperature.\n"
    "  DECISION: COMMIT\n"
    "  LABEL: hot=fever\n"
    "  REASON: Thermal context (90F, exertion) makes elevated body\n"
    "  temperature the dominant sense.\n"
    "\n"
    "EX5 - no pronoun, no ambiguity:\n"
    "Memory is empty. Input: \"set the table for dinner\". One interpretation\n"
    "with label \"default\".\n"
    "Checklist: no pronouns, one interpretation, no vague nouns.\n"
    "  DECISION: COMMIT\n"
    "  LABEL: default\n"
    "  REASON: Single interpretation; nothing to disambiguate.";

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

std::string render_interpretations(
        const std::vector<stylize::Interpretation> & interps) {
    std::string out;
    for (std::size_t i = 0; i < interps.size(); ++i) {
        out.append(std::to_string(i + 1));
        out.append(". LABEL: ");
        out.append(interps[i].label.empty() ? "default" : interps[i].label);
        out.append("\n   TEXT: ");
        out.append(interps[i].text);
        out.push_back('\n');
    }
    return out;
}

Decision parse(const std::string & raw,
               const std::vector<stylize::Interpretation> & interps) {
    Decision d;
    std::istringstream iss(raw);
    std::string line;
    enum { UNKNOWN, COMMIT, ASK } mode = UNKNOWN;

    while (std::getline(iss, line)) {
        const std::string t = trim(line);
        if (t.empty()) continue;
        if (starts_with_ci(t, "DECISION:")) {
            std::string v = trim(t.substr(9));
            if (starts_with_ci(v, "COMMIT")) mode = COMMIT;
            else if (starts_with_ci(v, "ASK")) mode = ASK;
        } else if (starts_with_ci(t, "LABEL:")) {
            d.chosen_label = trim(t.substr(6));
        } else if (starts_with_ci(t, "QUESTION:")) {
            d.question = trim(t.substr(9));
        } else if (starts_with_ci(t, "REASON:")) {
            d.reasoning = trim(t.substr(7));
        }
    }

    if (mode == ASK && !d.question.empty()) {
        d.needs_question = true;
        return d;
    }
    if (mode == COMMIT && !d.chosen_label.empty()) {
        // Validate label is one we offered; if not, snap to first.
        bool ok = false;
        for (const auto & i : interps) {
            const std::string lab = i.label.empty() ? "default" : i.label;
            if (lab == d.chosen_label) { ok = true; break; }
        }
        if (!ok && !interps.empty()) {
            d.chosen_label = interps[0].label.empty() ? "default" : interps[0].label;
            d.reasoning = "fallback: model returned unknown label; using first";
        }
        d.needs_question = false;
        return d;
    }

    // Hard fallback: commit to first interpretation.
    if (!interps.empty()) {
        d.needs_question = false;
        d.chosen_label   = interps[0].label.empty() ? "default" : interps[0].label;
        d.reasoning      = "fallback: unparseable decision; using first interpretation";
    }
    return d;
}

}

void init() {
    qwen14b::init();
}

Decision decide(std::string_view cleaned,
                const std::vector<stylize::Interpretation> & interpretations,
                std::string_view expertise) {
    std::string user_msg;
    user_msg.reserve(2048);
    user_msg.append("CLEANED INPUT: \"");
    user_msg.append(cleaned);
    user_msg.append("\"\n\n");

    user_msg.append("EXPERTISE: ");
    user_msg.append(expertise);
    user_msg.append("\n\n");

    user_msg.append("INTERPRETATIONS:\n");
    user_msg.append(render_interpretations(interpretations));
    user_msg.push_back('\n');

    user_msg.append("RECENT MEMORY (chronological):\n");
    user_msg.append(context::render_for_prompt(40));
    user_msg.push_back('\n');

    // Pull KB context for the cleaned input plus the most recent user answer
    // (if any). Helps the disambiguator resolve named entities mentioned in
    // either place. Renders "(knowledge offline)" when the ZIM is not yet
    // mounted, which is harmless.
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

    user_msg.append("Decide now.");

    const std::string raw =
        qwen14b::generate(kSystemPrompt, user_msg, /*max_new_tokens=*/256);
    return parse(raw, interpretations);
}

}
