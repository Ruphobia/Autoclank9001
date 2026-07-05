#include "test_runner.hpp"
#include "../modules/2124_field_pharmacy_reference/field_pharmacy_reference.hpp"

namespace {

testing::TestOutcome run_field_pharmacy_reference() {
    field_pharmacy_reference::init();
    auto s = field_pharmacy_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    field_pharmacy_reference::shutdown();
    return testing::ok();
}

const int _reg_field_pharmacy_reference = testing::register_test(
    "field_pharmacy_reference",
    "2124_field_pharmacy_reference: stub status check",
    &run_field_pharmacy_reference);

}
