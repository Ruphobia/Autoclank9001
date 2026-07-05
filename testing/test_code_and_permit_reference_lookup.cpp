#include "test_runner.hpp"
#include "../modules/2040_code_and_permit_reference_lookup/code_and_permit_reference_lookup.hpp"

namespace {

testing::TestOutcome run_code_and_permit_reference_lookup() {
    code_and_permit_reference_lookup::init();
    auto s = code_and_permit_reference_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    code_and_permit_reference_lookup::shutdown();
    return testing::ok();
}

const int _reg_code_and_permit_reference_lookup = testing::register_test(
    "code_and_permit_reference_lookup",
    "2040_code_and_permit_reference_lookup: stub status check",
    &run_code_and_permit_reference_lookup);

}
