#include "auction_price_comparator.hpp"

namespace auction_price_comparator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Auction Price Comparator (Numismatics, philately, collecting, antiques). Awaits wire-up.";
    return s;
}

}
