#include "test_runner.hpp"
#include "../modules/403_schematic_and_service_manual_archive/schematic_and_service_manual_archive.hpp"

namespace {

testing::TestOutcome run_schematic_and_service_manual_archive() {
    schematic_and_service_manual_archive::init();
    auto s = schematic_and_service_manual_archive::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    schematic_and_service_manual_archive::shutdown();
    return testing::ok();
}

const int _reg_schematic_and_service_manual_archive = testing::register_test(
    "schematic_and_service_manual_archive",
    "403_schematic_and_service_manual_archive: stub status check",
    &run_schematic_and_service_manual_archive);

}
