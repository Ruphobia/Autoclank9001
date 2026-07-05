#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Regex designer: tester + small set of dialect translation passes +
// AI-free synthesis seed from positive examples. The tester uses
// std::regex (ECMAScript) under the hood; translations are
// best-effort textual rewrites for the cross-dialect knobs that bite
// most often (named groups, possessive quantifiers, look-behind
// flavors). Real round-trip compilation across PCRE / RE2 / POSIX
// belongs in a layer that links those engines.
namespace regex_designer {

enum class Dialect { Ecma = 0, Pcre = 1, Re2 = 2, Posix = 3 };
const char * to_string(Dialect);

struct Capture {
    std::string  name;        // empty for unnamed groups
    size_t       start  = 0;
    size_t       length = 0;
    std::string  text;
};

struct MatchSpan {
    size_t                 start  = 0;
    size_t                 length = 0;
    std::string            text;
    std::vector<Capture>   captures;
};

struct TestResult {
    bool                    valid_pattern   = true;
    std::string             error;            // populated when valid_pattern==false
    std::vector<MatchSpan>  matches;
};

void init();
void shutdown();

// Test `pattern` against `sample`. case_insensitive applies the ECMA
// icase flag. `find_all` walks every non-overlapping match; otherwise
// returns the first hit only. Invalid pattern is reported in `error`
// without throwing.
TestResult test(std::string_view pattern,
                std::string_view sample,
                bool case_insensitive = false,
                bool find_all         = true);

// Translate the textual pattern across dialects. Currently handles:
//   - PCRE named groups "(?P<name>...)" <-> ECMA "(?<name>...)"
//   - PCRE possessive quantifiers "*+","++","?+","{n,m}+" -> non-possessive
//     when the target does not support them (degrades to greedy).
// Returns the original pattern unchanged when there is no rewrite
// applicable; the function never throws.
std::string translate(std::string_view pattern, Dialect from, Dialect to);

// Tiny pattern-synthesis seed: every positive example is matched
// literally as an alternation, but consecutive characters that
// vary across positives are coalesced into a character class /
// "." wildcard. Useful as a starting point for the user to refine.
// Returns the empty string when `positives` is empty.
std::string synthesize_from_examples(const std::vector<std::string> & positives);

}
