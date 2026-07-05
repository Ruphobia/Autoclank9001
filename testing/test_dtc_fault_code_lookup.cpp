#include "test_runner.hpp"
#include "../modules/1227_dtc_fault_code_lookup/dtc_fault_code_lookup.hpp"

namespace {

testing::TestOutcome run_dtc_fault_code_lookup() {
    dtc_fault_code_lookup::init();
    auto s = dtc_fault_code_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dtc_fault_code_lookup::shutdown();
    return testing::ok();
}

const int _reg_dtc_fault_code_lookup = testing::register_test(
    "dtc_fault_code_lookup",
    "1227_dtc_fault_code_lookup: stub status check",
    &run_dtc_fault_code_lookup);

}
