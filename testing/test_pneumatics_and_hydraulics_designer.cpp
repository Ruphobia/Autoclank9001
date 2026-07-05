#include "test_runner.hpp"
#include "../modules/949_pneumatics_and_hydraulics_designer/pneumatics_and_hydraulics_designer.hpp"

namespace {

testing::TestOutcome run_pneumatics_and_hydraulics_designer() {
    pneumatics_and_hydraulics_designer::init();
    auto s = pneumatics_and_hydraulics_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pneumatics_and_hydraulics_designer::shutdown();
    return testing::ok();
}

const int _reg_pneumatics_and_hydraulics_designer = testing::register_test(
    "pneumatics_and_hydraulics_designer",
    "949_pneumatics_and_hydraulics_designer: stub status check",
    &run_pneumatics_and_hydraulics_designer);

}
