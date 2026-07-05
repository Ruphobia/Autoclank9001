#include "test_runner.hpp"

#include <utility>

namespace testing {

static std::vector<TestCase> & mut_registry() {
    static std::vector<TestCase> r;
    return r;
}

int register_test(const std::string & name,
                  const std::string & description,
                  TestFn               fn) {
    mut_registry().push_back({name, description, std::move(fn)});
    return 0;
}

const std::vector<TestCase> & registry() { return mut_registry(); }

TestOutcome ok()                              { return {Result::OK,   {}}; }
TestOutcome fail(const std::string & message) { return {Result::FAIL, message}; }
TestOutcome skip(const std::string & reason)  { return {Result::SKIP, reason}; }

}
