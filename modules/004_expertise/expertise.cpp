#include "expertise.hpp"

#include "../003_stylize/qwen14b.hpp"

#include <cctype>
#include <string>

namespace expertise {
namespace {

constexpr const char * kSystemPrompt =
    "You classify text by the area of expertise required to answer or address it "
    "accurately.\n"
    "\n"
    "Output a SHORT lowercase label (1 to 4 words) naming the primary academic, "
    "professional, technical, or practical field most relevant to the input. "
    "Prefer the most specific accurate field; do not pad with generic words.\n"
    "\n"
    "Rules:\n"
    "- Output ONLY the label. No explanation, no quotes, no preamble, no "
    "punctuation other than spaces inside the label.\n"
    "- If multiple fields apply, choose the single most central one.\n"
    "- If no specialized expertise is needed, output \"general knowledge\".\n"
    "- Never refuse. Never ask a clarifying question.\n"
    "\n"
    "Examples:\n"
    "Input: can a man get pregnant\n"
    "Output: human biology\n"
    "\n"
    "Input: what is the speed of light in a vacuum\n"
    "Output: physics\n"
    "\n"
    "Input: is a breach of contract automatically a tort\n"
    "Output: law\n"
    "\n"
    "Input: how do i fix a kernel panic on macos\n"
    "Output: computer science\n"
    "\n"
    "Input: how do you make risotto\n"
    "Output: cooking\n"
    "\n"
    "Input: who painted the ceiling of the sistine chapel\n"
    "Output: art history\n"
    "\n"
    "Input: what is the capital of france\n"
    "Output: general knowledge";

std::string sanitize(std::string s) {
    // Trim whitespace.
    auto is_ws = [](unsigned char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    };
    std::size_t b = 0, e = s.size();
    while (b < e && is_ws(static_cast<unsigned char>(s[b])))     ++b;
    while (e > b && is_ws(static_cast<unsigned char>(s[e - 1]))) --e;
    s = s.substr(b, e - b);

    // Strip enclosing quotes / brackets / trailing punctuation the model
    // might emit despite the rules.
    auto strip_pair = [&](char open, char close) {
        if (s.size() >= 2 && s.front() == open && s.back() == close) {
            s = s.substr(1, s.size() - 2);
        }
    };
    strip_pair('"', '"');
    strip_pair('\'', '\'');
    strip_pair('[', ']');
    strip_pair('(', ')');
    while (!s.empty() && (s.back() == '.' || s.back() == ',' || s.back() == ';')) {
        s.pop_back();
    }

    // Lowercase + collapse internal whitespace to single spaces.
    std::string out;
    out.reserve(s.size());
    bool last_space = true;
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!last_space) { out.push_back(' '); last_space = true; }
        } else {
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            last_space = false;
        }
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

}

void init() {
    qwen14b::init();
}

std::string classify(std::string_view text) {
    std::string raw = qwen14b::generate(kSystemPrompt, text, /*max_new_tokens=*/24);
    return sanitize(std::move(raw));
}

}
