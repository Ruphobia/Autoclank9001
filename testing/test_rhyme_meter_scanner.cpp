#include "test_runner.hpp"
#include "../modules/800_rhyme_meter_scanner/rhyme_meter_scanner.hpp"

namespace {

testing::TestOutcome run_rhyme_meter_scanner() {
    rhyme_meter_scanner::init();
    auto s = rhyme_meter_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rhyme_meter_scanner::shutdown();
    return testing::ok();
}

const int _reg_rhyme_meter_scanner = testing::register_test(
    "rhyme_meter_scanner",
    "800_rhyme_meter_scanner: stub status check",
    &run_rhyme_meter_scanner);

}
