#include "test_runner.hpp"
#include "../modules/705_morphological_parser/morphological_parser.hpp"

namespace {

testing::TestOutcome run_morphological_parser() {
    morphological_parser::init();
    auto s = morphological_parser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    morphological_parser::shutdown();
    return testing::ok();
}

const int _reg_morphological_parser = testing::register_test(
    "morphological_parser",
    "705_morphological_parser: stub status check",
    &run_morphological_parser);

}
