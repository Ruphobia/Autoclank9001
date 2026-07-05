// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/079_pdf_ingestor/pdf_ingestor.hpp"

namespace {

testing::TestOutcome run_pdf_ingestor() {
    pdf_ingestor::init();
    auto s = pdf_ingestor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pdf_ingestor::shutdown();
    return testing::ok();
}

const int _reg_pdf_ingestor = testing::register_test(
    "pdf_ingestor",
    "079_pdf_ingestor: stub status check",
    &run_pdf_ingestor);

}
