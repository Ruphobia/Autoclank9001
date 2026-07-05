#include "exit_ticket_designer.hpp"

namespace exit_ticket_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Exit Ticket Designer (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
