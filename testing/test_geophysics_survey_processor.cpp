#include "test_runner.hpp"
#include "../modules/1537_geophysics_survey_processor/geophysics_survey_processor.hpp"

namespace {

testing::TestOutcome run_geophysics_survey_processor() {
    geophysics_survey_processor::init();
    auto s = geophysics_survey_processor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    geophysics_survey_processor::shutdown();
    return testing::ok();
}

const int _reg_geophysics_survey_processor = testing::register_test(
    "geophysics_survey_processor",
    "1537_geophysics_survey_processor: stub status check",
    &run_geophysics_survey_processor);

}
