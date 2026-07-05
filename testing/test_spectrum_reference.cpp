#include "test_runner.hpp"
#include "../modules/922_spectrum_reference/spectrum_reference.hpp"

namespace {

testing::TestOutcome run_spectrum_reference() {
    spectrum_reference::init();
    auto s = spectrum_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spectrum_reference::shutdown();
    return testing::ok();
}

const int _reg_spectrum_reference = testing::register_test(
    "spectrum_reference",
    "922_spectrum_reference: stub status check",
    &run_spectrum_reference);

}
