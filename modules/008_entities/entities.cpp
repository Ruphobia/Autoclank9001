#include "entities.hpp"

#include "../003_stylize/qwen14b.hpp"

#include <cctype>
#include <cstring>
#include <sstream>
#include <string>

namespace entities {
namespace {

constexpr const char * kSystemPrompt =
    "You extract named entity mentions from text and canonicalize each to "
    "its public-knowledge form using your own world knowledge.\n"
    "\n"
    "ENTITY = a public person, place, work, brand, event, fictional "
    "character, organization, or similar. Skip generic nouns (\"the dog\", "
    "\"my friend\") and skip private individuals you have no info on.\n"
    "\n"
    "CANONICALIZE AGGRESSIVELY. Treat the user's spelling as a phonetic / "
    "typo'd HINT, not authoritative. You know the canonical name from "
    "world knowledge -- use that. Examples of input -> canonical:\n"
    "  pamala handerson         -> Pamela Anderson\n"
    "  pamela handersson        -> Pamela Anderson\n"
    "  babewatch / babe watch   -> Baywatch\n"
    "  einsteen / einstien      -> Albert Einstein\n"
    "  sherlok homes            -> Sherlock Holmes\n"
    "  zucherburg               -> Mark Zuckerberg\n"
    "  the simson               -> The Simpsons\n"
    "  newyork                  -> New York City\n"
    "\n"
    "OUTPUT FORMAT - one line per resolved entity, then END. No preamble, "
    "no commentary, no markdown. If no resolvable entities: emit only END.\n"
    "\n"
    "ENTITY: <original substring> || <canonical name> || <one-sentence summary>\n"
    "...\n"
    "END\n"
    "\n"
    "Be terse in the summary (~10-20 words). Use a single canonical form "
    "per entity. Do not emit an ENTITY line for any mention you can't "
    "confidently identify.\n"
    "\n"
    "EXAMPLE INPUT: \"pamala handerson in that show babewatch\"\n"
    "EXAMPLE OUTPUT:\n"
    "ENTITY: pamala handerson || Pamela Anderson || Canadian-American actress and model best known for Baywatch (1992-1997)\n"
    "ENTITY: babewatch || Baywatch || American action drama television series about Los Angeles County lifeguards, 1989-2001\n"
    "END\n"
    "\n"
    "EXAMPLE INPUT: \"my coworker beth said the weather is hot\"\n"
    "EXAMPLE OUTPUT:\n"
    "END";

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
    const std::size_t n = std::strlen(prefix);
    if (s.size() < n) return false;
    for (std::size_t i = 0; i < n; ++i) {
        if (std::tolower(static_cast<unsigned char>(s[i])) !=
            std::tolower(static_cast<unsigned char>(prefix[i]))) return false;
    }
    return true;
}

std::vector<Mention> parse(const std::string & raw) {
    std::vector<Mention> out;
    std::istringstream iss(raw);
    std::string line;
    while (std::getline(iss, line)) {
        std::string t = trim(line);
        if (t.empty()) continue;
        if (starts_with_ci(t, "END")) break;
        if (!starts_with_ci(t, "ENTITY:")) continue;
        std::string body = trim(t.substr(7));
        // Split on "||"
        auto pos1 = body.find("||");
        if (pos1 == std::string::npos) continue;
        auto pos2 = body.find("||", pos1 + 2);
        if (pos2 == std::string::npos) continue;
        Mention m;
        m.original  = trim(body.substr(0, pos1));
        m.canonical = trim(body.substr(pos1 + 2, pos2 - (pos1 + 2)));
        m.summary   = trim(body.substr(pos2 + 2));
        if (m.original.empty() || m.canonical.empty()) continue;
        out.push_back(std::move(m));
    }
    return out;
}

}

void init() {
    qwen14b::init();
}

std::vector<Mention> extract(std::string_view text) {
    const std::string raw =
        qwen14b::generate(kSystemPrompt, text, /*max_new_tokens=*/384);
    return parse(raw);
}

}
