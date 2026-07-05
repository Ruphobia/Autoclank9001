#include "test_runner.hpp"
#include "../modules/425_pdf_annotator/pdf_annotator.hpp"

namespace {

testing::TestOutcome run_pdf_annotator() {
    pdf_annotator::init();
    auto s = pdf_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pdf_annotator::shutdown();
    return testing::ok();
}

const int _reg_pdf_annotator = testing::register_test(
    "pdf_annotator",
    "425_pdf_annotator: stub status check",
    &run_pdf_annotator);

}
