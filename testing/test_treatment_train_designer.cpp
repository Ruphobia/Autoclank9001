#include "test_runner.hpp"
#include "../modules/1066_treatment_train_designer/treatment_train_designer.hpp"

namespace {

testing::TestOutcome run_treatment_train_designer() {
    treatment_train_designer::init();
    auto s = treatment_train_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    treatment_train_designer::shutdown();
    return testing::ok();
}

const int _reg_treatment_train_designer = testing::register_test(
    "treatment_train_designer",
    "1066_treatment_train_designer: stub status check",
    &run_treatment_train_designer);

}
