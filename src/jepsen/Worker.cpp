#include "Worker.h"

namespace jepsen {

void Worker::run() {
    fut = std::async(std::launch::async, &Worker::runAsync, this);
}

Operation Worker::runAsync() {
    bool flag = true;
    SetThreadName(getName().c_str());

    log4cplus::NDC& ndc = log4cplus::getNDC();
    log4cplus::tstring name(t_name.begin(), t_name.end());
    log4cplus::NDCContextCreator _first_ndc(name);
    if(id != kNemesisProcess) {
        LOG4CPLUS_INFO(logger, getName().c_str() << " Start to connect to node" << id);
    } else {
        LOG4CPLUS_INFO(logger, getName().c_str() << " Start to setup Nemesis");
    }

    while (flag) {
        Operation op;
        // TODO: try_dequeue? wait_dequeue? take? poll?
        if (in->try_dequeue(op)) {
            LOG4CPLUS_INFO(logger, op.toString().c_str());
            switch (op.type) {
                case Operation::kExit:
                    flag = false;
                    break;
                // TODO: Case sleep
                // TODO: Case log
                default: {
                    invoke(op);
                    LOG4CPLUS_INFO(logger, op.toString().c_str());
                    out->enqueue(op);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }

    close();
    LOG4CPLUS_INFO(logger, getName().c_str() << " finish in node" << id);
    ndc.remove();
    return OperationFactory::exit();
}

bool ClientWorker::open(int id) {
    return true;
}

bool ClientWorker::invoke(Operation& op) {
    // TODO: resuable?
    // LOG4CPLUS_INFO(logger, "process = " << process << ", op.process = " << op.process);
    if (process == kInvalidProcess || process != op.process) {
        // LOG4CPLUS_INFO(logger, "Close and retart a client");
        close();
        client = Client::createOne();
        client->open(node);
        process = op.process;
    }
    // LOG4CPLUS_INFO(logger, "continue");
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