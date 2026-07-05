// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1656_model_hub/model_hub.hpp"

namespace {

testing::TestOutcome run_model_hub() {
    model_hub::init();
    auto s = model_hub::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    model_hub::shutdown();
    return testing::ok();
}

const int _reg_model_hub = testing::register_test(
    "model_hub",
    "1656_model_hub: stub status check",
    &run_model_hub);

}
