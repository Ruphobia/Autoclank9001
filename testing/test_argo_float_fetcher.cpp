// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1554_argo_float_fetcher/argo_float_fetcher.hpp"

namespace {

testing::TestOutcome run_argo_float_fetcher() {
    argo_float_fetcher::init();
    auto s = argo_float_fetcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    argo_float_fetcher::shutdown();
    return testing::ok();
}

const int _reg_argo_float_fetcher = testing::register_test(
    "argo_float_fetcher",
    "1554_argo_float_fetcher: stub status check",
    &run_argo_float_fetcher);

}
