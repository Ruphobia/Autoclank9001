// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/576_patent_litigation_docket/patent_litigation_docket.hpp"

namespace {

testing::TestOutcome run_patent_litigation_docket() {
    patent_litigation_docket::init();
    auto s = patent_litigation_docket::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patent_litigation_docket::shutdown();
    return testing::ok();
}

const int _reg_patent_litigation_docket = testing::register_test(
    "patent_litigation_docket",
    "576_patent_litigation_docket: stub status check",
    &run_patent_litigation_docket);

}
