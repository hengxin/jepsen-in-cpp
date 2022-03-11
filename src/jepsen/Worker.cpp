#include "Worker.h"

bool ClientWorker::open(int id) {
    return true;
}
bool ClientWorker::invoke(Operation& op) {
    return true;
}

bool ClientWorker::close() {
    return true;
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