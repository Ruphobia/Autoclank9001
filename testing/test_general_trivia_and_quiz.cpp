#include "test_runner.hpp"
#include "../modules/2200_general_trivia_and_quiz/general_trivia_and_quiz.hpp"

namespace {

testing::TestOutcome run_general_trivia_and_quiz() {
    general_trivia_and_quiz::init();
    auto s = general_trivia_and_quiz::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    general_trivia_and_quiz::shutdown();
    return testing::ok();
}

const int _reg_general_trivia_and_quiz = testing::register_test(
    "general_trivia_and_quiz",
    "2200_general_trivia_and_quiz: stub status check",
    &run_general_trivia_and_quiz);

}
