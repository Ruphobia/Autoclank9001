#include "test_runner.hpp"
#include "../modules/2159_chronotype_quiz/chronotype_quiz.hpp"

namespace {

testing::TestOutcome run_chronotype_quiz() {
    chronotype_quiz::init();
    auto s = chronotype_quiz::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    chronotype_quiz::shutdown();
    return testing::ok();
}

const int _reg_chronotype_quiz = testing::register_test(
    "chronotype_quiz",
    "2159_chronotype_quiz: stub status check",
    &run_chronotype_quiz);

}
