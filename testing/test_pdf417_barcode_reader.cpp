// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/601_pdf417_barcode_reader/pdf417_barcode_reader.hpp"

namespace {

testing::TestOutcome run_pdf417_barcode_reader() {
    pdf417_barcode_reader::init();
    auto s = pdf417_barcode_reader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pdf417_barcode_reader::shutdown();
    return testing::ok();
}

const int _reg_pdf417_barcode_reader = testing::register_test(
    "pdf417_barcode_reader",
    "601_pdf417_barcode_reader: stub status check",
    &run_pdf417_barcode_reader);

}
