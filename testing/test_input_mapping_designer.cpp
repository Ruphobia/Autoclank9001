#include "test_runner.hpp"
#include "../modules/306_input_mapping_designer/input_mapping_designer.hpp"

namespace {

testing::TestOutcome run_input_mapping_designer() {
    input_mapping_designer::init();
    auto s = input_mapping_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    input_mapping_designer::shutdown();
    return testing::ok();
}

const int _reg_input_mapping_designer = testing::register_test(
    "input_mapping_designer",
    "306_input_mapping_designer: stub status check",
    &run_input_mapping_designer);

}
