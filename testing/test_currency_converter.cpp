#include "test_runner.hpp"
#include "../modules/561_currency_converter/currency_converter.hpp"

namespace {

testing::TestOutcome run_currency_converter() {
    currency_converter::init();
    auto s = currency_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    currency_converter::shutdown();
    return testing::ok();
}

const int _reg_currency_converter = testing::register_test(
    "currency_converter",
    "561_currency_converter: stub status check",
    &run_currency_converter);

}
