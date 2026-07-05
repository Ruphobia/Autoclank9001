// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/282_syscall_and_io_tracer/syscall_and_io_tracer.hpp"

namespace {

testing::TestOutcome run_syscall_and_io_tracer() {
    syscall_and_io_tracer::init();
    auto s = syscall_and_io_tracer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    syscall_and_io_tracer::shutdown();
    return testing::ok();
}

const int _reg_syscall_and_io_tracer = testing::register_test(
    "syscall_and_io_tracer",
    "282_syscall_and_io_tracer: stub status check",
    &run_syscall_and_io_tracer);

}
