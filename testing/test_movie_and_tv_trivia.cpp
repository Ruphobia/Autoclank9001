#include "test_runner.hpp"
#include "../modules/2199_movie_and_tv_trivia/movie_and_tv_trivia.hpp"

namespace {

testing::TestOutcome run_movie_and_tv_trivia() {
    movie_and_tv_trivia::init();
    auto s = movie_and_tv_trivia::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    movie_and_tv_trivia::shutdown();
    return testing::ok();
}

const int _reg_movie_and_tv_trivia = testing::register_test(
    "movie_and_tv_trivia",
    "2199_movie_and_tv_trivia: stub status check",
    &run_movie_and_tv_trivia);

}
