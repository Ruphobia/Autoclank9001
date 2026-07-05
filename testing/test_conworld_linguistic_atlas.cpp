#include "test_runner.hpp"
#include "../modules/815_conworld_linguistic_atlas/conworld_linguistic_atlas.hpp"

namespace {

testing::TestOutcome run_conworld_linguistic_atlas() {
    conworld_linguistic_atlas::init();
    auto s = conworld_linguistic_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    conworld_linguistic_atlas::shutdown();
    return testing::ok();
}

const int _reg_conworld_linguistic_atlas = testing::register_test(
    "conworld_linguistic_atlas",
    "815_conworld_linguistic_atlas: stub status check",
    &run_conworld_linguistic_atlas);

}
