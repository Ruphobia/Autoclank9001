// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/175_vector_asset_converter/vector_asset_converter.hpp"

namespace {

testing::TestOutcome run_vector_asset_converter() {
    vector_asset_converter::init();
    auto s = vector_asset_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vector_asset_converter::shutdown();
    return testing::ok();
}

const int _reg_vector_asset_converter = testing::register_test(
    "vector_asset_converter",
    "175_vector_asset_converter: stub status check",
    &run_vector_asset_converter);

}
