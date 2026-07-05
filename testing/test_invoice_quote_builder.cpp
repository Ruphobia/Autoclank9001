// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/554_invoice_quote_builder/invoice_quote_builder.hpp"

namespace {

testing::TestOutcome run_invoice_quote_builder() {
    invoice_quote_builder::init();
    auto s = invoice_quote_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    invoice_quote_builder::shutdown();
    return testing::ok();
}

const int _reg_invoice_quote_builder = testing::register_test(
    "invoice_quote_builder",
    "554_invoice_quote_builder: stub status check",
    &run_invoice_quote_builder);

}
