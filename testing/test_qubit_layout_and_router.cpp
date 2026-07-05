// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/340_qubit_layout_and_router/qubit_layout_and_router.hpp"

namespace {

testing::TestOutcome run_qubit_layout_and_router() {
    qubit_layout_and_router::init();
    auto s = qubit_layout_and_router::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    qubit_layout_and_router::shutdown();
    return testing::ok();
}

const int _reg_qubit_layout_and_router = testing::register_test(
    "qubit_layout_and_router",
    "340_qubit_layout_and_router: stub status check",
    &run_qubit_layout_and_router);

}
