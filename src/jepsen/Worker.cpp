#include "Worker.h"

namespace jepsen {

void Worker::run() {
    fut = std::async(&Worker::runAsync, this);
}

Operation Worker::runAsync() {
    bool flag = true;
    while (flag) {
        Operation op;
        // TODO: try_dequeue? wait_dequeue? take? poll?
        if (in->try_dequeue(op)) {
            switch (op.type) {
                case Operation::kExit:
                    flag = true;
                    break;
                // TODO: Case sleep
                // TODO: Case log
                default: {
                    LOG4CPLUS_INFO(logger, op.toString().c_str());
                    client->invoke(op);
                    LOG4CPLUS_INFO(logger, op.toString().c_str());
                    out->try_enqueue(op);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }

    close();
}

bool ClientWorker::open(int id) {}

bool ClientWorker::invoke(Operation& op) {
    // TODO: resuable?
    if (process == kInvalidProcess || process != op.process) {
        close();
        client = Client::createOne();
        client->open(node);
        process = op.process;
    }
    return client->invoke(op);
}

bool ClientWorker::close() {
    if (client != nullptr) {
        client->close();
        client = nullptr;
    }
    return true;
}

bool NemesisWorker::open(int id) {
    return true;
}
bool NemesisWorker::invoke(Operation& op) {
    return client->invoke(op);
}

bool NemesisWorker::close() {
    return true;
}
}  // namespace jepsen