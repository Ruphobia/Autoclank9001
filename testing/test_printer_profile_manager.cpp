#include "test_runner.hpp"
#include "../modules/966_printer_profile_manager/printer_profile_manager.hpp"

namespace {

testing::TestOutcome run_printer_profile_manager() {
    printer_profile_manager::init();
    auto s = printer_profile_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    printer_profile_manager::shutdown();
    return testing::ok();
}

const int _reg_printer_profile_manager = testing::register_test(
    "printer_profile_manager",
    "966_printer_profile_manager: stub status check",
    &run_printer_profile_manager);

}
