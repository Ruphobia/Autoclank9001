// Smoke test for 066_stack_trace.

#include "test_runner.hpp"
#include "../modules/066_stack_trace/stack_trace.hpp"

namespace {

const char * kPy = R"PY(Traceback (most recent call last):
  File "/home/x/app.py", line 42, in main
    process(items)
  File "/home/x/app.py", line 17, in process
    raise ValueError("boom")
ValueError: boom
)PY";

const char * kNode = R"JS(TypeError: cannot read property foo of undefined
    at Object.handle (/srv/app/handler.js:21:13)
    at /srv/app/index.js:7:5
    at processTicksAndRejections (node:internal/process/task_queues:96:5)
)JS";

const char * kJava = R"JAVA(java.lang.NullPointerException
	at com.example.App.handle(App.java:42)
	at com.example.App.main(App.java:9)
)JAVA";

const char * kGo = R"GO(panic: runtime error: index out of range [3] with length 2
goroutine 1 [running]:
main.do(...)
        /src/main.go:21 +0xab
main.main()
        /src/main.go:9 +0x33
)GO";

const char * kRust = R"RUST(thread 'main' panicked at index out of bounds, src/main.rs:42:5
note: run with `RUST_BACKTRACE=1`
stack backtrace:
   0: std::panic
             at /rust/src/libstd/panic.rs:84:7
   1: my_app::run
             at src/lib.rs:7:13
)RUST";

testing::TestOutcome run() {
    stack_trace::init();

    auto py = stack_trace::parse_python(kPy);
    if (py.frames.size() != 2) return testing::fail("python frame count != 2");
    if (py.frames[0].file != "/home/x/app.py" || py.frames[0].line != 42 ||
        py.frames[0].function != "main")
        return testing::fail("python frame[0] wrong");
    if (py.error.find("ValueError") == std::string::npos)
        return testing::fail("python error missing");

    auto js = stack_trace::parse_nodejs(kNode);
    if (js.frames.size() < 2) return testing::fail("nodejs frame count < 2");
    if (js.frames[0].file.find("handler.js") == std::string::npos ||
        js.frames[0].line != 21 || js.frames[0].column != 13 ||
        js.frames[0].function != "Object.handle")
        return testing::fail("nodejs frame[0] wrong");
    if (js.error.find("TypeError") == std::string::npos)
        return testing::fail("nodejs error missing");

    auto j = stack_trace::parse_java(kJava);
    if (j.frames.size() != 2) return testing::fail("java frame count != 2");
    if (j.frames[0].file != "App.java" || j.frames[0].line != 42 ||
        j.frames[0].function != "com.example.App.handle")
        return testing::fail("java frame[0] wrong");

    auto g = stack_trace::parse_go(kGo);
    if (g.frames.size() < 2) return testing::fail("go frame count < 2");
    if (g.frames[0].file != "/src/main.go" || g.frames[0].line != 21)
        return testing::fail("go frame[0] wrong: " + g.frames[0].file + ":" + std::to_string(g.frames[0].line));
    if (g.error.find("index out of range") == std::string::npos)
        return testing::fail("go panic message missing");

    auto r = stack_trace::parse_rust(kRust);
    if (r.frames.empty()) return testing::fail("rust no frames");
    bool found_lib = false;
    for (const auto & f : r.frames)
        if (f.file == "src/lib.rs" && f.line == 7) found_lib = true;
    if (!found_lib) return testing::fail("rust src/lib.rs frame missing");
    if (r.error.find("index out of bounds") == std::string::npos)
        return testing::fail("rust panic msg missing");

    // Sniffer dispatches by content.
    if (stack_trace::parse(kPy).language   != stack_trace::Language::Python)
        return testing::fail("sniff missed python");
    if (stack_trace::parse(kJava).language != stack_trace::Language::Java)
        return testing::fail("sniff missed java");
    if (stack_trace::parse(kRust).language != stack_trace::Language::Rust)
        return testing::fail("sniff missed rust");

    stack_trace::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "stack_trace",
    "066_stack_trace: parse python / node / java / go / rust tracebacks",
    &run);

}
