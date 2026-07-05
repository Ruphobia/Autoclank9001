// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1224_vin_decoder_and_service_notebook/vin_decoder_and_service_notebook.hpp"

namespace {

testing::TestOutcome run_vin_decoder_and_service_notebook() {
    vin_decoder_and_service_notebook::init();
    auto s = vin_decoder_and_service_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vin_decoder_and_service_notebook::shutdown();
    return testing::ok();
}

const int _reg_vin_decoder_and_service_notebook = testing::register_test(
    "vin_decoder_and_service_notebook",
    "1224_vin_decoder_and_service_notebook: stub status check",
    &run_vin_decoder_and_service_notebook);

}
