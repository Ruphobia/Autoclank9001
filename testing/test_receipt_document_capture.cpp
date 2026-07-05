#include "test_runner.hpp"
#include "../modules/555_receipt_document_capture/receipt_document_capture.hpp"

namespace {

testing::TestOutcome run_receipt_document_capture() {
    receipt_document_capture::init();
    auto s = receipt_document_capture::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    receipt_document_capture::shutdown();
    return testing::ok();
}

const int _reg_receipt_document_capture = testing::register_test(
    "receipt_document_capture",
    "555_receipt_document_capture: stub status check",
    &run_receipt_document_capture);

}
