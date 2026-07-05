#include "test_runner.hpp"
#include "../modules/1798_flavor_aroma_pairing_explorer/flavor_aroma_pairing_explorer.hpp"

namespace {

testing::TestOutcome run_flavor_aroma_pairing_explorer() {
    flavor_aroma_pairing_explorer::init();
    auto s = flavor_aroma_pairing_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    flavor_aroma_pairing_explorer::shutdown();
    return testing::ok();
}

const int _reg_flavor_aroma_pairing_explorer = testing::register_test(
    "flavor_aroma_pairing_explorer",
    "1798_flavor_aroma_pairing_explorer: stub status check",
    &run_flavor_aroma_pairing_explorer);

}
