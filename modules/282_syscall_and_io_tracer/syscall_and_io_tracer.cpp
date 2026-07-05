#include "syscall_and_io_tracer.hpp"

namespace syscall_and_io_tracer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Syscall and IO tracer (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
