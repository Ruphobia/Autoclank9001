#include "test_runner.hpp"
#include "../modules/1838_escapement_designer/escapement_designer.hpp"

namespace {

testing::TestOutcome run_escapement_designer() {
    escapement_designer::init();
    auto s = escapement_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    escapement_designer::shutdown();
    return testing::ok();
}

const int _reg_escapement_designer = testing::register_test(
    "escapement_designer",
    "1838_escapement_designer: stub status check",
    &run_escapement_designer);

}
