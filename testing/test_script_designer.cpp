#include "test_runner.hpp"
#include "../modules/811_script_designer/script_designer.hpp"

namespace {

testing::TestOutcome run_script_designer() {
    script_designer::init();
    auto s = script_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    script_designer::shutdown();
    return testing::ok();
}

const int _reg_script_designer = testing::register_test(
    "script_designer",
    "811_script_designer: stub status check",
    &run_script_designer);

}
