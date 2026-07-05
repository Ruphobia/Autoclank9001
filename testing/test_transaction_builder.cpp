#include "test_runner.hpp"
#include "../modules/385_transaction_builder/transaction_builder.hpp"

namespace {

testing::TestOutcome run_transaction_builder() {
    transaction_builder::init();
    auto s = transaction_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    transaction_builder::shutdown();
    return testing::ok();
}

const int _reg_transaction_builder = testing::register_test(
    "transaction_builder",
    "385_transaction_builder: stub status check",
    &run_transaction_builder);

}
