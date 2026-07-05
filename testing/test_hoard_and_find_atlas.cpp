#include "test_runner.hpp"
#include "../modules/1892_hoard_and_find_atlas/hoard_and_find_atlas.hpp"

namespace {

testing::TestOutcome run_hoard_and_find_atlas() {
    hoard_and_find_atlas::init();
    auto s = hoard_and_find_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hoard_and_find_atlas::shutdown();
    return testing::ok();
}

const int _reg_hoard_and_find_atlas = testing::register_test(
    "hoard_and_find_atlas",
    "1892_hoard_and_find_atlas: stub status check",
    &run_hoard_and_find_atlas);

}
