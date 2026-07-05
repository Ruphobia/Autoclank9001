#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace stylize {

struct Interpretation {
    // Short note naming the key sense disambiguation, e.g. "hot=attractive",
    // "bank=financial-institution", or "default" when the input is unambiguous.
    std::string label;
    // The full Spock/Coneheads precision rewrite under this interpretation.
    std::string text;
};

void init();
void shutdown();

// Returns one Interpretation per plausible reading of the input (up to ~4).
// Unambiguous inputs return a single entry with label "default". A future
// context-aware layer is expected to pick one Interpretation downstream.
std::vector<Interpretation> precise(std::string_view cleaned,
                                    std::string_view defs);

// Produces the FINAL single rewrite after disambiguation has resolved the
// chosen sense and the user clarifications. Reads recent memory to bind
// every pronoun / vague referent to its actual entity (using both memory
// and world knowledge). Output is one line, same Spock/Coneheads voice as
// precise(), but with NO "unspecified" placeholder phrasing.
std::string render_final(std::string_view cleaned,
                         std::string_view chosen_label,
                         std::string_view defs);

// Rewrites `cleaned` with pronouns and deictics ("they", "there", "it",
// "that one") replaced by their explicit referents from recent session
// memory, changing nothing else. render_final() resolves referents too,
// but too late: the KB query, lookup-intent detection, and the coder all
// consume the text before the render exists. Returns `cleaned` unchanged
// when there is nothing to resolve or the model output is untrustworthy.
// Callers should gate on "text contains a referring word AND the session
// has prior turns" to avoid a pointless model call.
std::string resolve_referents(std::string_view cleaned);

}
