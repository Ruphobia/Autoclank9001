#include "test_runner.hpp"
#include "../modules/1689_rotoscope_studio/rotoscope_studio.hpp"

namespace {

testing::TestOutcome run_rotoscope_studio() {
    rotoscope_studio::init();
    auto s = rotoscope_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rotoscope_studio::shutdown();
    return testing::ok();
}

const int _reg_rotoscope_studio = testing::register_test(
    "rotoscope_studio",
    "1689_rotoscope_studio: stub status check",
    &run_rotoscope_studio);

}
