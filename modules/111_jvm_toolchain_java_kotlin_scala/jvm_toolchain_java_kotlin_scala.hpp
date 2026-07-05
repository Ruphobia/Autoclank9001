#pragma once

#include <string>

// Stub module for the "JVM toolchain (Java, Kotlin, Scala)" tool entry.
// Per-language toolchains.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace jvm_toolchain_java_kotlin_scala {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
