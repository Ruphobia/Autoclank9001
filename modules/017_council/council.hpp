#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

// Cross-domain Socratic council. Orchestrates several specialists in a
// structured dialogue against each other on one multi-domain prompt:
// every specialist sees the prompt plus the previous round's
// contributions, returns its own, and a coordinator computes a
// convergence metric and stops when the specialists have stopped
// drifting. Synthesis is then the latest round, attributed.
//
// Specialists are callable from outside (the bundled physics, chemistry,
// generalist answer modules wrap nicely into a council::Specialist) but
// nothing here loads a model; the council does not touch a GPU. Test
// harnesses can plug in pure-string mock specialists.
namespace council {

using Generator = std::function<std::string(std::string_view rendered_prompt)>;

struct Specialist {
    std::string name;       // short identifier, e.g. "physics"
    std::string domain;     // human-facing label, e.g. "physics / electronics"
    Generator   generate;   // takes the rendered (prompt + prior round) text
};

struct Contribution {
    std::string specialist_name;
    std::string text;
};

struct Round {
    std::vector<Contribution> contributions;
    double                    convergence = 0.0;   // 0..1 (mean pairwise Jaccard)
};

struct Verdict {
    std::vector<Round> rounds;
    std::string        synthesis;       // markdown, attributed
    double             final_convergence = 0.0;
    bool               converged         = false;  // hit the threshold
};

// Run a council. `max_rounds >= 1`. `convergence_threshold` in [0,1];
// the council stops early when a round's mean pairwise Jaccard meets
// or exceeds the threshold. Each round renders the prompt as:
//
//     <original prompt>
//
//     --- previous round contributions ---
//     [<name1>] <text1>
//     [<name2>] <text2>
//     ...
//
// and hands it to every specialist's generate() in parallel-equivalent
// (sequential) order.
Verdict run(std::string_view                   prompt,
            const std::vector<Specialist>    & specialists,
            int                                max_rounds            = 3,
            double                             convergence_threshold = 0.6);

// Compute mean pairwise Jaccard similarity over the content-word bags
// of `texts`. 0 when fewer than 2 texts. Exposed because the chat UI
// may want to draw a "specialist agreement" bar without re-running.
double mean_pairwise_jaccard(const std::vector<std::string> & texts);

}
