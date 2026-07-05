#include "test_runner.hpp"
#include "../modules/2140_probate_and_land_parser/probate_and_land_parser.hpp"

namespace {

testing::TestOutcome run_probate_and_land_parser() {
    probate_and_land_parser::init();
    auto s = probate_and_land_parser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    probate_and_land_parser::shutdown();
    return testing::ok();
}

const int _reg_probate_and_land_parser = testing::register_test(
    "probate_and_land_parser",
    "2140_probate_and_land_parser: stub status check",
    &run_probate_and_land_parser);

}
