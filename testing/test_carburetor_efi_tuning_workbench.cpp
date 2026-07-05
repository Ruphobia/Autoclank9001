#include "test_runner.hpp"
#include "../modules/1237_carburetor_efi_tuning_workbench/carburetor_efi_tuning_workbench.hpp"

namespace {

testing::TestOutcome run_carburetor_efi_tuning_workbench() {
    carburetor_efi_tuning_workbench::init();
    auto s = carburetor_efi_tuning_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    carburetor_efi_tuning_workbench::shutdown();
    return testing::ok();
}

const int _reg_carburetor_efi_tuning_workbench = testing::register_test(
    "carburetor_efi_tuning_workbench",
    "1237_carburetor_efi_tuning_workbench: stub status check",
    &run_carburetor_efi_tuning_workbench);

}
