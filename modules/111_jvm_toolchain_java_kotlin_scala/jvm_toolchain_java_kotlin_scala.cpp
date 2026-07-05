#include "jvm_toolchain_java_kotlin_scala.hpp"

namespace jvm_toolchain_java_kotlin_scala {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: JVM toolchain (Java, Kotlin, Scala) (Per-language toolchains). Awaits wire-up.";
    return s;
}

}
