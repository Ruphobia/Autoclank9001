#include "test_runner.hpp"
#include "../modules/685_calendar_converter/calendar_converter.hpp"

namespace {

testing::TestOutcome run_calendar_converter() {
    calendar_converter::init();
    auto s = calendar_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    calendar_converter::shutdown();
    return testing::ok();
}

const int _reg_calendar_converter = testing::register_test(
    "calendar_converter",
    "685_calendar_converter: stub status check",
    &run_calendar_converter);

}
