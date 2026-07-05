#include "test_runner.hpp"
#include "../modules/1612_compost_recipe_builder/compost_recipe_builder.hpp"

namespace {

testing::TestOutcome run_compost_recipe_builder() {
    compost_recipe_builder::init();
    auto s = compost_recipe_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    compost_recipe_builder::shutdown();
    return testing::ok();
}

const int _reg_compost_recipe_builder = testing::register_test(
    "compost_recipe_builder",
    "1612_compost_recipe_builder: stub status check",
    &run_compost_recipe_builder);

}
