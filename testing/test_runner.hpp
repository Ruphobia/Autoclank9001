#pragma once

#include <functional>
#include <string>
#include <vector>

// Standalone smoke-test framework for the tool_test binary. Each module
// registers one CLI-level smoke test via a static initializer in its own
// translation unit; the harness has no compile-time list of modules.
//
// A test does five things:
//   1. initialize the module under test (or skip if prerequisites missing)
//   2. exercise its primary entry point with one realistic minimal input
//   3. assert one or two output properties
//   4. shut down cleanly
//   5. return testing::ok() / testing::skip(reason) / testing::fail(message)
//
// The runner catches exceptions; a throwing test is reported as FAIL.

namespace testing {

enum class Result { OK = 0, FAIL = 1, SKIP = 2 };

struct TestOutcome {
    Result      result;
    std::string message;
};

using TestFn = std::function<TestOutcome()>;

struct TestCase {
    std::string name;
    std::string description;
    TestFn      fn;
};

// Register a test. Returns a dummy int so callers can write:
//   static const int _r = testing::register_test(name, desc, &fn);
// Safe to call from a static initializer. Registration order is the
// order the linker placed the translation units.
int register_test(const std::string & name,
                  const std::string & description,
                  TestFn               fn);

const std::vector<TestCase> & registry();

TestOutcome ok();
TestOutcome fail(const std::string & message);
TestOutcome skip(const std::string & reason);

}
