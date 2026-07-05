#include "test_runner.hpp"
#include "../modules/1155_vortex_lattice_method/vortex_lattice_method.hpp"

namespace {

testing::TestOutcome run_vortex_lattice_method() {
    vortex_lattice_method::init();
    auto s = vortex_lattice_method::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vortex_lattice_method::shutdown();
    return testing::ok();
}

const int _reg_vortex_lattice_method = testing::register_test(
    "vortex_lattice_method",
    "1155_vortex_lattice_method: stub status check",
    &run_vortex_lattice_method);

}
