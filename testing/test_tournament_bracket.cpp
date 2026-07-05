#include "test_runner.hpp"
#include "../modules/1996_tournament_bracket/tournament_bracket.hpp"

namespace {

testing::TestOutcome run_tournament_bracket() {
    tournament_bracket::init();
    auto s = tournament_bracket::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tournament_bracket::shutdown();
    return testing::ok();
}

const int _reg_tournament_bracket = testing::register_test(
    "tournament_bracket",
    "1996_tournament_bracket: stub status check",
    &run_tournament_bracket);

}
