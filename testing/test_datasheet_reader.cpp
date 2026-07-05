#include "test_runner.hpp"
#include "../modules/857_datasheet_reader/datasheet_reader.hpp"

namespace {

testing::TestOutcome run_datasheet_reader() {
    datasheet_reader::init();
    auto s = datasheet_reader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    datasheet_reader::shutdown();
    return testing::ok();
}

const int _reg_datasheet_reader = testing::register_test(
    "datasheet_reader",
    "857_datasheet_reader: stub status check",
    &run_datasheet_reader);

}
