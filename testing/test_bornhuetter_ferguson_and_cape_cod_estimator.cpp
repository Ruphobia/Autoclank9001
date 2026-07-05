#include "test_runner.hpp"
#include "../modules/544_bornhuetter_ferguson_and_cape_cod_estimator/bornhuetter_ferguson_and_cape_cod_estimator.hpp"

namespace {

testing::TestOutcome run_bornhuetter_ferguson_and_cape_cod_estimator() {
    bornhuetter_ferguson_and_cape_cod_estimator::init();
    auto s = bornhuetter_ferguson_and_cape_cod_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bornhuetter_ferguson_and_cape_cod_estimator::shutdown();
    return testing::ok();
}

const int _reg_bornhuetter_ferguson_and_cape_cod_estimator = testing::register_test(
    "bornhuetter_ferguson_and_cape_cod_estimator",
    "544_bornhuetter_ferguson_and_cape_cod_estimator: stub status check",
    &run_bornhuetter_ferguson_and_cape_cod_estimator);

}
