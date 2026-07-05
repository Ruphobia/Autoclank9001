#include "test_runner.hpp"
#include "../modules/1376_metabolic_model_workbench/metabolic_model_workbench.hpp"

namespace {

testing::TestOutcome run_metabolic_model_workbench() {
    metabolic_model_workbench::init();
    auto s = metabolic_model_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    metabolic_model_workbench::shutdown();
    return testing::ok();
}

const int _reg_metabolic_model_workbench = testing::register_test(
    "metabolic_model_workbench",
    "1376_metabolic_model_workbench: stub status check",
    &run_metabolic_model_workbench);

}
