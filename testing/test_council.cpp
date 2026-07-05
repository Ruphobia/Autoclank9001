// Smoke test for 017_council: orchestrate three mock specialists,
// confirm rounds accumulate, convergence threshold short-circuits,
// and the synthesis names every specialist.

#include "test_runner.hpp"
#include "../modules/017_council/council.hpp"

#include <string>

namespace {

testing::TestOutcome run() {
    // Three specialists. Two of them agree from round 1; the third
    // drifts. With a permissive threshold the council still runs to
    // max_rounds because the disagreeing third drags the mean down.
    council::Specialist a;
    a.name = "alpha"; a.domain = "general";
    a.generate = [](std::string_view) {
        return std::string("water boils at one hundred degrees celsius at standard pressure");
    };
    council::Specialist b;
    b.name = "beta"; b.domain = "chemistry";
    b.generate = [](std::string_view) {
        return std::string("water boils at one hundred degrees celsius standard atmospheric pressure");
    };
    council::Specialist c;
    c.name = "gamma"; c.domain = "physics";
    static int round_ix = 0;
    c.generate = [](std::string_view) {
        ++round_ix;
        if (round_ix == 1) return std::string("boiling depends on altitude and atmospheric pressure");
        return std::string("water boils at one hundred degrees celsius standard atmospheric pressure altitude");
    };

    auto v = council::run("At what temperature does water boil?", {a, b, c},
                          3, 0.5);

    if (v.rounds.empty()) return testing::fail("no rounds recorded");
    if (v.rounds[0].contributions.size() != 3)
        return testing::fail("round 0 missing specialists");
    if (v.synthesis.find("alpha") == std::string::npos ||
        v.synthesis.find("beta")  == std::string::npos ||
        v.synthesis.find("gamma") == std::string::npos)
        return testing::fail("synthesis missing specialist headers");

    // Round 1 had c drifting; round 2 c aligned. Convergence should rise.
    if (v.rounds.size() >= 2) {
        if (v.rounds[1].convergence < v.rounds[0].convergence)
            return testing::fail("convergence didn't rise after agreement");
    }

    // Pure-agreement specialists: should hit the threshold in round 0.
    council::Specialist same1, same2;
    same1.name = "x"; same2.name = "y";
    same1.generate = same2.generate = [](std::string_view) {
        return std::string("identical body for convergence test");
    };
    auto agree = council::run("p", {same1, same2}, 3, 0.5);
    if (!agree.converged) return testing::fail("identical specialists did not converge");
    if (agree.rounds.size() != 1)
        return testing::fail("identical specialists ran past round 0");

    // Jaccard sanity.
    double j = council::mean_pairwise_jaccard({"alpha beta gamma", "alpha beta delta"});
    if (j <= 0.0 || j >= 1.0) return testing::fail("pairwise jaccard out of range");
    double j_self = council::mean_pairwise_jaccard({"alpha beta", "alpha beta"});
    if (j_self < 0.99) return testing::fail("identical bags didn't return ~1.0");

    return testing::ok();
}

const int _r = testing::register_test(
    "council",
    "017_council: rounds, convergence threshold, synthesis attribution",
    &run);

}
