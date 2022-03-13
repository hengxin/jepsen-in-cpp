#include "Worker.h"

namespace jepsen {

bool ClientWorker::open(int id) {
    return client->open(node);
}
bool ClientWorker::invoke(Operation& op) {
    return client->invoke(op);
}

bool ClientWorker::close() {
    return client->close();
}

bool NemesisWorker::open(int id) {
    return true;
}
bool NemesisWorker::invoke(Operation& op) {
    return true;
}

bool NemesisWorker::close() {
    return true;
}
}  // namespace jepsen