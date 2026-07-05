#include "test_runner.hpp"
#include "../modules/725_codicology_inspector/codicology_inspector.hpp"

namespace {

testing::TestOutcome run_codicology_inspector() {
    codicology_inspector::init();
    auto s = codicology_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    codicology_inspector::shutdown();
    return testing::ok();
}

const int _reg_codicology_inspector = testing::register_test(
    "codicology_inspector",
    "725_codicology_inspector: stub status check",
    &run_codicology_inspector);

}
