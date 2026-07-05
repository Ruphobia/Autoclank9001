#pragma once

#include "../003_stylize/stylize.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace disambiguate {

struct Decision {
    bool        needs_question = false;
    std::string question;        // when needs_question == true
    std::string chosen_label;    // when needs_question == false
    std::string reasoning;       // model's short rationale
};

// Idempotent; readies the underlying 14B model (shared with stylize).
void init();

// Look at the input, the candidate interpretations, the expertise tag, and
// the recent session memory. Decide either to COMMIT to one interpretation
// label or to ASK ONE clarifying question (never more than one at a time).
//
// Memory is read by this function. The caller writes the resulting question
// (when ASK) and the user's reply back into memory so the next decide()
// call sees it.
Decision decide(std::string_view cleaned,
                const std::vector<stylize::Interpretation> & interpretations,
                std::string_view expertise);

}
