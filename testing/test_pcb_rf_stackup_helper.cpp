#include "test_runner.hpp"
#include "../modules/1196_pcb_rf_stackup_helper/pcb_rf_stackup_helper.hpp"

namespace {

testing::TestOutcome run_pcb_rf_stackup_helper() {
    pcb_rf_stackup_helper::init();
    auto s = pcb_rf_stackup_helper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pcb_rf_stackup_helper::shutdown();
    return testing::ok();
}

const int _reg_pcb_rf_stackup_helper = testing::register_test(
    "pcb_rf_stackup_helper",
    "1196_pcb_rf_stackup_helper: stub status check",
    &run_pcb_rf_stackup_helper);

}
