#include "test_runner.hpp"
#include "../modules/1830_beerrecipedesigner/beerrecipedesigner.hpp"

namespace {

testing::TestOutcome run_beerrecipedesigner() {
    beerrecipedesigner::init();
    auto s = beerrecipedesigner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    beerrecipedesigner::shutdown();
    return testing::ok();
}

const int _reg_beerrecipedesigner = testing::register_test(
    "beerrecipedesigner",
    "1830_beerrecipedesigner: stub status check",
    &run_beerrecipedesigner);

}
