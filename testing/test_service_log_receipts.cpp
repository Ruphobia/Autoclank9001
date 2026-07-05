#include "test_runner.hpp"
#include "../modules/1228_service_log_receipts/service_log_receipts.hpp"

namespace {

testing::TestOutcome run_service_log_receipts() {
    service_log_receipts::init();
    auto s = service_log_receipts::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    service_log_receipts::shutdown();
    return testing::ok();
}

const int _reg_service_log_receipts = testing::register_test(
    "service_log_receipts",
    "1228_service_log_receipts: stub status check",
    &run_service_log_receipts);

}
