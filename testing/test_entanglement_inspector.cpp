#include "test_runner.hpp"
#include "../modules/346_entanglement_inspector/entanglement_inspector.hpp"

namespace {

testing::TestOutcome run_entanglement_inspector() {
    entanglement_inspector::init();
    auto s = entanglement_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    entanglement_inspector::shutdown();
    return testing::ok();
}

const int _reg_entanglement_inspector = testing::register_test(
    "entanglement_inspector",
    "346_entanglement_inspector: stub status check",
    &run_entanglement_inspector);

}
