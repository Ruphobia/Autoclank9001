// SPDX-License-Identifier: GPL-3.0-or-later
#include "classify.hpp"

#include "../003_stylize/qwen14b.hpp"

#include <cctype>
#include <cstring>
#include <sstream>
#include <string>

namespace classify {
namespace {

constexpr const char * kSystemPrompt =
    "You classify a user request by its speech act. Output EXACTLY three "
    "lines and nothing else (no preface, no commentary, no markdown):\n"
    "\n"
    "ACT: <one of: command, question, statement, acknowledgment, correction>\n"
    "SUBTYPE: <subtype from the list for that act>\n"
    "TAGS: <comma-separated flags or \"none\">\n"
    "\n"
    "PRIMARY ACTS:\n"
    "- command         : user wants action taken on the local system / via a tool\n"
    "- question        : user wants information back\n"
    "- statement       : user is asserting a fact, opinion, or observation "
    "with NO action wanted\n"
    "- acknowledgment  : short content-light reply (\"ok\", \"yes\", \"thanks\", \"got it\")\n"
    "- correction      : user is fixing / overriding something said earlier\n"
    "\n"
    "IMPORTANT: words describing a DESIRED STATE of files, folders, code, "
    "or configuration (\"X should be in Y\", \"A needs to listen on port "
    "B\", \"that belongs in src/\") are a command to make it so -- "
    "especially right after work was done -- NOT a statement.\n"
    "\n"
    "SUBTYPES per primary:\n"
    "- command:        shell | code | web | compute | generic\n"
    "- question:       factual | procedural | opinion | yes-no | hypothetical\n"
    "- statement:      personal-fact | world-fact | mood | observation\n"
    "- acknowledgment: confirm | thanks | agreement\n"
    "- correction:     content | target\n"
    "\n"
    "TAGS (apply any that fit, comma-separated, or \"none\"):\n"
    "- destructive          : command removes/overwrites/uninstalls\n"
    "- needs-web            : question requires live / post-cutoff info\n"
    "- needs-shell-info     : question's answer comes from running a local command\n"
    "- persistent           : statement worth long-term remembering\n"
    "- contradicts-memory   : statement or correction disagrees with prior memory\n"
    "- urgent               : explicit urgency in the request\n"
    "- multi-part           : request bundles multiple acts\n"
    "\n"
    "EXAMPLES:\n"
    "\n"
    "USER: create a folder in ~/work named test\n"
    "ACT: command\n"
    "SUBTYPE: shell\n"
    "TAGS: none\n"
    "\n"
    "USER: rm -rf /tmp/old_logs\n"
    "ACT: command\n"
    "SUBTYPE: shell\n"
    "TAGS: destructive\n"
    "\n"
    "USER: what is the capital of france\n"
    "ACT: question\n"
    "SUBTYPE: factual\n"
    "TAGS: none\n"
    "\n"
    "USER: how much free disk space do I have\n"
    "ACT: question\n"
    "SUBTYPE: procedural\n"
    "TAGS: needs-shell-info\n"
    "\n"
    "USER: what was the score of the laker game last night\n"
    "ACT: question\n"
    "SUBTYPE: factual\n"
    "TAGS: needs-web\n"
    "\n"
    "USER: I'm a nurse who works night shifts\n"
    "ACT: statement\n"
    "SUBTYPE: personal-fact\n"
    "TAGS: persistent\n"
    "\n"
    "USER: this code is broken\n"
    "ACT: statement\n"
    "SUBTYPE: observation\n"
    "TAGS: none\n"
    "\n"
    "USER: the webserver should be in the 001_interface folder\n"
    "ACT: command\n"
    "SUBTYPE: shell\n"
    "TAGS: none\n"
    "\n"
    "USER: main.cpp needs to listen on port 9001, not 8080\n"
    "ACT: command\n"
    "SUBTYPE: code\n"
    "TAGS: none\n"
    "\n"
    "USER: ok\n"
    "ACT: acknowledgment\n"
    "SUBTYPE: confirm\n"
    "TAGS: none\n"
    "\n"
    "USER: thanks\n"
    "ACT: acknowledgment\n"
    "SUBTYPE: thanks\n"
    "TAGS: none\n"
    "\n"
    "USER: no that's wrong, her name is Pamela not Paula\n"
    "ACT: correction\n"
    "SUBTYPE: content\n"
    "TAGS: contradicts-memory\n"
    "\n"
    "- Never use an em dash (U+2014), en dash (U+2013), or horizontal bar "
    "(U+2015). Use a plain hyphen (U+002D) when a dash is needed. This is a "
    "hard operator policy.\n";

std::string trim(std::string s) {
    auto is_ws = [](unsigned char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
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

std::vector<std::string> split_csv(std::string s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == ',') {
            std::string t = trim(cur);
            if (!t.empty() && t != "none") out.push_back(t);
            cur.clear();
        } else cur.push_back(c);
    }
    std::string t = trim(cur);
    if (!t.empty() && t != "none") out.push_back(t);
    return out;
}

}

void init() { qwen14b::init(); }

Result analyze(std::string_view cleaned, std::string_view rewrite) {
    std::string user_msg;
    user_msg.reserve(cleaned.size() + rewrite.size() + 32);
    user_msg.append("USER: ");
    user_msg.append(cleaned);
    if (!rewrite.empty()) {
        user_msg.append("\n(disambiguated form: ");
        user_msg.append(rewrite);
        user_msg.append(")");
    }

    const std::string raw =
        qwen14b::generate(kSystemPrompt, user_msg, /*max_new_tokens=*/64);

    Result r;
    std::istringstream iss(raw);
    std::string line;
    while (std::getline(iss, line)) {
        std::string t = trim(line);
        if (t.empty()) continue;
        if      (starts_with_ci(t, "ACT:"))     r.act     = trim(t.substr(4));
        else if (starts_with_ci(t, "SUBTYPE:")) r.subtype = trim(t.substr(8));
        else if (starts_with_ci(t, "TAGS:"))    r.tags    = split_csv(trim(t.substr(5)));
    }

    // Lower-case the primary act so downstream comparisons are stable.
    for (char & c : r.act)     c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    for (char & c : r.subtype) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return r;
}

std::string format_tags(const Result & r) {
    std::string out = "[act=";
    out.append(r.act.empty() ? "unknown" : r.act);
    if (!r.subtype.empty()) {
        out.append(" subtype=");
        out.append(r.subtype);
    }
    if (!r.tags.empty()) {
        out.append(" tags=");
        for (std::size_t i = 0; i < r.tags.size(); ++i) {
            if (i) out.append(",");
            out.append(r.tags[i]);
        }
    }
    out.append("]");
    return out;
}

}
