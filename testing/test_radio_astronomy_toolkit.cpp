#include "test_runner.hpp"
#include "../modules/1495_radio_astronomy_toolkit/radio_astronomy_toolkit.hpp"

namespace {

testing::TestOutcome run_radio_astronomy_toolkit() {
    radio_astronomy_toolkit::init();
    auto s = radio_astronomy_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    radio_astronomy_toolkit::shutdown();
    return testing::ok();
}

const int _reg_radio_astronomy_toolkit = testing::register_test(
    "radio_astronomy_toolkit",
    "1495_radio_astronomy_toolkit: stub status check",
    &run_radio_astronomy_toolkit);

}
