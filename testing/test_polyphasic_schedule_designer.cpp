#include "test_runner.hpp"
#include "../modules/2171_polyphasic_schedule_designer/polyphasic_schedule_designer.hpp"

namespace {

testing::TestOutcome run_polyphasic_schedule_designer() {
    polyphasic_schedule_designer::init();
    auto s = polyphasic_schedule_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polyphasic_schedule_designer::shutdown();
    return testing::ok();
}

const int _reg_polyphasic_schedule_designer = testing::register_test(
    "polyphasic_schedule_designer",
    "2171_polyphasic_schedule_designer: stub status check",
    &run_polyphasic_schedule_designer);

}
