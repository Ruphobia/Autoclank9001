#include "test_runner.hpp"
#include "../modules/1809_modified_atmosphere_packaging_reference/modified_atmosphere_packaging_reference.hpp"

namespace {

testing::TestOutcome run_modified_atmosphere_packaging_reference() {
    modified_atmosphere_packaging_reference::init();
    auto s = modified_atmosphere_packaging_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    modified_atmosphere_packaging_reference::shutdown();
    return testing::ok();
}

const int _reg_modified_atmosphere_packaging_reference = testing::register_test(
    "modified_atmosphere_packaging_reference",
    "1809_modified_atmosphere_packaging_reference: stub status check",
    &run_modified_atmosphere_packaging_reference);

}
