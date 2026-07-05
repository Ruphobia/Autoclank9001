#include "websearch.hpp"

#include "../../003_stylize/qwen14b.hpp"

#include <algorithm>
#include <cctype>
#include <string>

namespace websearch {
namespace {

constexpr const char * kSystemPrompt =
    "You are an intent detector for a local developer assistant. Decide "
    "whether fulfilling the user's request needs an OUTBOUND WEB LOOKUP "
    "(a DuckDuckGo search, possibly followed by downloading a resource).\n"
    "\n"
    "Answer LOOKUP: required ONLY when the request is IMPOSSIBLE without "
    "the network:\n"
    "- it asks to find, download, install, vendor, or integrate a "
    "third-party library, package, framework, plugin, font, dataset, or "
    "asset that must be OBTAINED (the assistant cannot write a real "
    "library like a charting engine from scratch);\n"
    "- it asks a factual question whose answer depends on current or "
    "post-training-cutoff information (latest version, today's price, "
    "recent release). Stable encyclopedic facts (capitals, historical "
    "dates, definitions, science) are NEVER 'required': the assistant "
    "has a local Wikipedia and answers them offline.\n"
    "\n"
    "Answer LOOKUP: helpful when the work is doable locally but a search "
    "might improve it (examples or reference material would be nice to "
    "have, not necessary).\n"
    "\n"
    "Answer LOOKUP: no for ordinary local work the assistant can do "
    "entirely from its own knowledge: writing servers, web pages, "
    "templates, styles, or code from scratch; editing existing files; "
    "building; running; filesystem actions. Writing a basic web page or "
    "a hello-world anything is ALWAYS 'no': that is generation, not "
    "retrieval.\n"
    "\n"
    "Respond with EXACTLY three lines and nothing else:\n"
    "LOOKUP: required|helpful|no\n"
    "QUERY: <the web search query to run, or - if no>\n"
    "REASON: <one short clause>\n"
    "\n"
    "EXAMPLES:\n"
    "USER: find and integrate a candlestick charting javascript library "
    "and make a demo page\n"
    "LOOKUP: required\n"
    "QUERY: lightweight javascript candlestick chart library\n"
    "REASON: a real charting library must be downloaded\n"
    "\n"
    "USER: add the webserver, and have a basic dark themed hello world "
    "web page\n"
    "LOOKUP: no\n"
    "QUERY: -\n"
    "REASON: server and page can be written from scratch\n"
    "\n"
    "USER: what is the latest stable version of nginx\n"
    "LOOKUP: required\n"
    "QUERY: latest stable nginx version\n"
    "REASON: post-cutoff factual lookup\n"
    "\n"
    "USER: what is the capital of france\n"
    "LOOKUP: no\n"
    "QUERY: -\n"
    "REASON: stable encyclopedic fact; the local knowledge base covers it\n"
    "\n"
    "USER: what kind of food do people eat in Paris, France?\n"
    "LOOKUP: no\n"
    "QUERY: -\n"
    "REASON: stable cultural knowledge; local sources cover it\n"
    "\n"
    "USER: add a dark theme to the existing index.html\n"
    "LOOKUP: no\n"
    "QUERY: -\n"
    "REASON: local edit, no external resource\n"
    "\n"
    "USER: build a login page that follows current best practices for "
    "password UX\n"
    "LOOKUP: helpful\n"
    "QUERY: password field UX best practices\n"
    "REASON: doable locally; current guidance would improve it\n"
    "\n"
    "USER: refactor main.cpp to split the server into its own file\n"
    "LOOKUP: no\n"
    "QUERY: -\n"
    "REASON: local refactor\n";

std::string lc(std::string_view sv) {
    std::string s(sv);
    for (auto & c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

std::string trim(const std::string & s) {
    std::size_t b = 0, e = s.size();
    auto ws = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    while (b < e && ws(s[b])) ++b;
    while (e > b && ws(s[e - 1])) --e;
    return s.substr(b, e - b);
}

// Value of the first line that BEGINS with `key` (case-insensitive,
// leading whitespace ignored). A plain substring find would also match
// the key inside an echoed format spec or a REASON sentence.
std::string line_value(const std::string & body, const std::string & key) {
    const std::string lk = lc(key);
    std::size_t pos = 0;
    while (pos < body.size()) {
        std::size_t e = body.find('\n', pos);
        if (e == std::string::npos) e = body.size();
        std::string line = trim(body.substr(pos, e - pos));
        if (lc(line).rfind(lk, 0) == 0) {
            return trim(line.substr(lk.size()));
        }
        pos = e + 1;
    }
    return {};
}

// Shared keyword lists: does the text read like "obtain an external
// artifact"? Used by the offline heuristic AND as corroboration before
// a model 'required' verdict may hard-block a command turn.
constexpr const char * kObtainVerbs[] = {
    "download", "install", "integrate", "vendor", "fetch ", "add a ",
    "find a ", "find me", "get a ", "grab a ", "pull in", "look up",
    "search for", "search the web",
};
constexpr const char * kObtainNouns[] = {
    "library", "lib ", "package", "framework", "plugin", "cdn",
    "chart", "charting", "font", "icon", "widget", "dependency",
    "npm", "github", "latest version",
    // Common web-asset extensions: "download the min.js version" and
    // "get the .css file" name the target by its file suffix, not by a
    // dictionary category.
    ".js", ".css", ".ttf", ".otf", ".woff", ".woff2", ".json",
    ".ico", ".svg", ".png", ".jpg", ".jpeg", ".webp", ".map",
};
// A "strong obtain verb" is unambiguous on its own: "download" and its
// wget/curl aliases name a concrete network fetch the agent must perform,
// regardless of what the target noun is. Without this, "download the
// min.js version" got downgraded to a helpful lookup because the noun
// list did not include the target's name.
constexpr const char * kStrongObtainVerbs[] = {
    "download ", "wget ", "curl the ", "curl -o", "curl --output",
};

bool obtain_keywords_match(std::string_view cleaned) {
    const std::string s = lc(cleaned);
    for (auto v : kStrongObtainVerbs) {
        if (s.find(v) != std::string::npos) return true;
    }
    bool has_verb = false, has_noun = false;
    for (auto v : kObtainVerbs) if (s.find(v) != std::string::npos) has_verb = true;
    for (auto n : kObtainNouns) if (s.find(n) != std::string::npos) has_noun = true;
    return has_verb && has_noun;
}

// Keyword heuristic used when the model is unavailable or unparseable.
Intent heuristic(std::string_view cleaned, bool classify_needs_web) {
    Intent it;
    it.needs_lookup = obtain_keywords_match(cleaned) || classify_needs_web;
    if (it.needs_lookup) {
        it.query  = std::string(cleaned).substr(0, 120);
        it.reason = "keyword heuristic (model unavailable)";
    }
    return it;
}

}  // namespace

Intent detect_intent(std::string_view cleaned, bool classify_needs_web) {
    std::string out;
    try {
        out = qwen14b::generate(kSystemPrompt, cleaned, /*max_new_tokens=*/96);
    } catch (...) {
        return heuristic(cleaned, classify_needs_web);
    }
    if (trim(out).empty()) return heuristic(cleaned, classify_needs_web);

    Intent it;
    const std::string lookup = lc(line_value(out, "LOOKUP:"));
    // Leading-token matching, negations first: a substring test here once
    // turned "not required" into a hard refusal of local work.
    if (lookup.rfind("no", 0) == 0) {
        // "no", "none", "not required", "not needed" -- all mean no lookup.
    } else if (lookup.rfind("required", 0) == 0) {
        it.needs_lookup = true;
        it.required     = true;
    } else if (lookup.rfind("helpful", 0) == 0 ||
               lookup.rfind("yes", 0) == 0) {
        // "yes" (older phrasing) maps to helpful: a detector false
        // positive must never hard-block local work.
        it.needs_lookup = true;
        it.required     = false;
    }
    it.query  = line_value(out, "QUERY:");
    it.reason = line_value(out, "REASON:");
    if (it.query == "-" || it.query.empty()) it.query.clear();

    // The existing needs-web classifier is a prior, but only ever at the
    // "helpful" tier: it cannot make a lookup mandatory on its own.
    if (!it.needs_lookup && classify_needs_web) {
        it.needs_lookup = true;
        it.required     = false;
        if (it.query.empty()) it.query = std::string(cleaned).substr(0, 120);
        if (it.reason.empty()) it.reason = "classifier flagged needs-web";
    }
    // needs_lookup with no usable query is not actionable.
    if (it.needs_lookup && it.query.empty()) {
        it.query = std::string(cleaned).substr(0, 120);
    }
    if (it.reason.empty()) it.reason = it.needs_lookup ? "lookup detected" : "no lookup needed";
    return it;
}

bool obtain_intent(std::string_view cleaned) {
    return obtain_keywords_match(cleaned);
}

}  // namespace websearch
