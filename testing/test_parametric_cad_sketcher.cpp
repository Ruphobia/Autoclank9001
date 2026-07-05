// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/925_parametric_cad_sketcher/parametric_cad_sketcher.hpp"

namespace {

testing::TestOutcome run_parametric_cad_sketcher() {
    parametric_cad_sketcher::init();
    auto s = parametric_cad_sketcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    parametric_cad_sketcher::shutdown();
    return testing::ok();
}

const int _reg_parametric_cad_sketcher = testing::register_test(
    "parametric_cad_sketcher",
    "925_parametric_cad_sketcher: stub status check",
    &run_parametric_cad_sketcher);

}
