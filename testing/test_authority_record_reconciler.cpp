// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/695_authority_record_reconciler/authority_record_reconciler.hpp"

namespace {

testing::TestOutcome run_authority_record_reconciler() {
    authority_record_reconciler::init();
    auto s = authority_record_reconciler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    authority_record_reconciler::shutdown();
    return testing::ok();
}

const int _reg_authority_record_reconciler = testing::register_test(
    "authority_record_reconciler",
    "695_authority_record_reconciler: stub status check",
    &run_authority_record_reconciler);

}
