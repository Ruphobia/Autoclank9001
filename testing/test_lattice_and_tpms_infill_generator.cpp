#include "test_runner.hpp"
#include "../modules/971_lattice_and_tpms_infill_generator/lattice_and_tpms_infill_generator.hpp"

namespace {

testing::TestOutcome run_lattice_and_tpms_infill_generator() {
    lattice_and_tpms_infill_generator::init();
    auto s = lattice_and_tpms_infill_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lattice_and_tpms_infill_generator::shutdown();
    return testing::ok();
}

const int _reg_lattice_and_tpms_infill_generator = testing::register_test(
    "lattice_and_tpms_infill_generator",
    "971_lattice_and_tpms_infill_generator: stub status check",
    &run_lattice_and_tpms_infill_generator);

}
