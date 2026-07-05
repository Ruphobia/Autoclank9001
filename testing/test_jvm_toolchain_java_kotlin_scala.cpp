#include "test_runner.hpp"
#include "../modules/111_jvm_toolchain_java_kotlin_scala/jvm_toolchain_java_kotlin_scala.hpp"

namespace {

testing::TestOutcome run_jvm_toolchain_java_kotlin_scala() {
    jvm_toolchain_java_kotlin_scala::init();
    auto s = jvm_toolchain_java_kotlin_scala::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    jvm_toolchain_java_kotlin_scala::shutdown();
    return testing::ok();
}

const int _reg_jvm_toolchain_java_kotlin_scala = testing::register_test(
    "jvm_toolchain_java_kotlin_scala",
    "111_jvm_toolchain_java_kotlin_scala: stub status check",
    &run_jvm_toolchain_java_kotlin_scala);

}
