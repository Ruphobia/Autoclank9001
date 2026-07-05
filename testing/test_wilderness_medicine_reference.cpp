#include "test_runner.hpp"
#include "../modules/2071_wilderness_medicine_reference/wilderness_medicine_reference.hpp"

namespace {

testing::TestOutcome run_wilderness_medicine_reference() {
    wilderness_medicine_reference::init();
    auto s = wilderness_medicine_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wilderness_medicine_reference::shutdown();
    return testing::ok();
}

const int _reg_wilderness_medicine_reference = testing::register_test(
    "wilderness_medicine_reference",
    "2071_wilderness_medicine_reference: stub status check",
    &run_wilderness_medicine_reference);

}
