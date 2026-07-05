#include "test_runner.hpp"
#include "../modules/2026_refrigerant_charge_analyzer/refrigerant_charge_analyzer.hpp"

namespace {

testing::TestOutcome run_refrigerant_charge_analyzer() {
    refrigerant_charge_analyzer::init();
    auto s = refrigerant_charge_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    refrigerant_charge_analyzer::shutdown();
    return testing::ok();
}

const int _reg_refrigerant_charge_analyzer = testing::register_test(
    "refrigerant_charge_analyzer",
    "2026_refrigerant_charge_analyzer: stub status check",
    &run_refrigerant_charge_analyzer);

}
