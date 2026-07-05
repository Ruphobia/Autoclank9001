// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/708_cuneiform_sign_reader/cuneiform_sign_reader.hpp"

namespace {

testing::TestOutcome run_cuneiform_sign_reader() {
    cuneiform_sign_reader::init();
    auto s = cuneiform_sign_reader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cuneiform_sign_reader::shutdown();
    return testing::ok();
}

const int _reg_cuneiform_sign_reader = testing::register_test(
    "cuneiform_sign_reader",
    "708_cuneiform_sign_reader: stub status check",
    &run_cuneiform_sign_reader);

}
