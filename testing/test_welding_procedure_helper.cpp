#include "test_runner.hpp"
#include "../modules/998_welding_procedure_helper/welding_procedure_helper.hpp"

namespace {

testing::TestOutcome run_welding_procedure_helper() {
    welding_procedure_helper::init();
    auto s = welding_procedure_helper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    welding_procedure_helper::shutdown();
    return testing::ok();
}

const int _reg_welding_procedure_helper = testing::register_test(
    "welding_procedure_helper",
    "998_welding_procedure_helper: stub status check",
    &run_welding_procedure_helper);

}
