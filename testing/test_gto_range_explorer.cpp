#include "test_runner.hpp"
#include "../modules/1971_gto_range_explorer/gto_range_explorer.hpp"

namespace {

testing::TestOutcome run_gto_range_explorer() {
    gto_range_explorer::init();
    auto s = gto_range_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gto_range_explorer::shutdown();
    return testing::ok();
}

const int _reg_gto_range_explorer = testing::register_test(
    "gto_range_explorer",
    "1971_gto_range_explorer: stub status check",
    &run_gto_range_explorer);

}
