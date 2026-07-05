#include "test_runner.hpp"
#include "../modules/123_type_generator/type_generator.hpp"

namespace {

testing::TestOutcome run_type_generator() {
    type_generator::init();
    auto s = type_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    type_generator::shutdown();
    return testing::ok();
}

const int _reg_type_generator = testing::register_test(
    "type_generator",
    "123_type_generator: stub status check",
    &run_type_generator);

}
