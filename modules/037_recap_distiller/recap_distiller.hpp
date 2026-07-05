#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "../024_memory_composer/composer.hpp"

// Session Recap Distiller: reads a session transcript and proposes
// candidate memory entries the user can accept, edit, or reject in
// one click. Heuristic-only here: we walk each user turn looking for
// trigger phrases that signal a decision, preference, or fact, and
// emit a 024_memory_composer::Draft per hit. An LLM-assisted distiller
// can wrap this layer later without changing the API.
namespace recap_distiller {

enum class Speaker { User = 0, Assistant = 1 };

struct Turn {
    Speaker      speaker = Speaker::User;
    std::string  text;
    int          index   = 0;   // 0-based ordinal in the session
    std::string  session_id;    // optional; copied into the source link
};

struct Candidate {
    memory_composer::Draft draft;
    std::string            trigger;     // the phrase that fired (for transparency)
    size_t                 turn_index;  // which turn it came from
};

void init();
void shutdown();

// Walk the transcript and return one Candidate per matched trigger.
// Default scope is Project; callers can override the draft.scope after
// distillation when they want a personal-scope capture.
std::vector<Candidate> distill(const std::vector<Turn> & transcript);

}
