#include "test_runner.hpp"
#include "../modules/848_bom_and_sourcing/bom_and_sourcing.hpp"

namespace {

testing::TestOutcome run_bom_and_sourcing() {
    bom_and_sourcing::init();
    auto s = bom_and_sourcing::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bom_and_sourcing::shutdown();
    return testing::ok();
}

const int _reg_bom_and_sourcing = testing::register_test(
    "bom_and_sourcing",
    "848_bom_and_sourcing: stub status check",
    &run_bom_and_sourcing);

}
