#include "test_runner.hpp"
#include "../modules/215_radio_link_designer/radio_link_designer.hpp"

namespace {

testing::TestOutcome run_radio_link_designer() {
    radio_link_designer::init();
    auto s = radio_link_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    radio_link_designer::shutdown();
    return testing::ok();
}

const int _reg_radio_link_designer = testing::register_test(
    "radio_link_designer",
    "215_radio_link_designer: stub status check",
    &run_radio_link_designer);

}
