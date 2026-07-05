#include "test_runner.hpp"
#include "../modules/679_interpretation_pipeline_inspector/interpretation_pipeline_inspector.hpp"

namespace {

testing::TestOutcome run_interpretation_pipeline_inspector() {
    interpretation_pipeline_inspector::init();
    auto s = interpretation_pipeline_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    interpretation_pipeline_inspector::shutdown();
    return testing::ok();
}

const int _reg_interpretation_pipeline_inspector = testing::register_test(
    "interpretation_pipeline_inspector",
    "679_interpretation_pipeline_inspector: stub status check",
    &run_interpretation_pipeline_inspector);

}
