#include "test_runner.hpp"
#include "../modules/1318_polymer_builder/polymer_builder.hpp"

namespace {

testing::TestOutcome run_polymer_builder() {
    polymer_builder::init();
    auto s = polymer_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polymer_builder::shutdown();
    return testing::ok();
}

const int _reg_polymer_builder = testing::register_test(
    "polymer_builder",
    "1318_polymer_builder: stub status check",
    &run_polymer_builder);

}
