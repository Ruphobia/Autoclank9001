// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1158_cfd_mesh_generator/cfd_mesh_generator.hpp"

namespace {

testing::TestOutcome run_cfd_mesh_generator() {
    cfd_mesh_generator::init();
    auto s = cfd_mesh_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cfd_mesh_generator::shutdown();
    return testing::ok();
}

const int _reg_cfd_mesh_generator = testing::register_test(
    "cfd_mesh_generator",
    "1158_cfd_mesh_generator: stub status check",
    &run_cfd_mesh_generator);

}
