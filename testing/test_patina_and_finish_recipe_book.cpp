#include "test_runner.hpp"
#include "../modules/1855_patina_and_finish_recipe_book/patina_and_finish_recipe_book.hpp"

namespace {

testing::TestOutcome run_patina_and_finish_recipe_book() {
    patina_and_finish_recipe_book::init();
    auto s = patina_and_finish_recipe_book::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patina_and_finish_recipe_book::shutdown();
    return testing::ok();
}

const int _reg_patina_and_finish_recipe_book = testing::register_test(
    "patina_and_finish_recipe_book",
    "1855_patina_and_finish_recipe_book: stub status check",
    &run_patina_and_finish_recipe_book);

}
