#include "test_runner.hpp"
#include "../modules/615_dsar_builder/dsar_builder.hpp"

namespace {

testing::TestOutcome run_dsar_builder() {
    dsar_builder::init();
    auto s = dsar_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dsar_builder::shutdown();
    return testing::ok();
}

const int _reg_dsar_builder = testing::register_test(
    "dsar_builder",
    "615_dsar_builder: stub status check",
    &run_dsar_builder);

}
