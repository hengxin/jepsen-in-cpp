#ifndef WORKER_H
#define WORKER_H

#include "Client.h"
#include "Nemesis.h"
#include "Operation.h"
#include <future>
#include <memory>

namespace jepsen {
class Worker;
using WorkerPtr = std::shared_ptr<Worker>;

class Worker {
public:
    Worker() = delete;
    Worker(int id, OperationQueuePtr out)
        : id(id),
          out(out),
          in(std::make_shared<OperationQueue>(OperationQueue(1))),
          logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {}
    virtual bool open(int id) = 0;
    virtual bool invoke(Operation& op) = 0;
    virtual bool close() = 0;
    void run();
    Operation runAsync();
    inline int getId() {
        return id;
    }

    bool putOp(const Operation& op) {
        return in->enqueue(op);
    }

    void exit() {
        fut.get();
    }

protected:
    int id;
    unique_ptr<Client> client;
    OperationQueuePtr in;
    OperationQueuePtr out;
    std::future<Operation> fut;
    log4cplus::Logger logger;
};

class ClientWorker : public Worker {
public:
    ClientWorker() = delete;
    ClientWorker(int id, OperationQueuePtr out, string node)
        : Worker(id, out), node(node), process(kInvalidProcess) {}
    virtual bool open(int id) override;
    virtual bool invoke(Operation& op) override;
    virtual bool close() override;

private:
    string node;
    int process;
};

class NemesisWorker : public Worker {
public:
    NemesisWorker() = delete;
    NemesisWorker(int id, OperationQueuePtr out) : Worker(id, out) {
        this->client = Nemesis::createOne();
    }
    virtual bool open(int id) override;
    virtual bool invoke(Operation& op) override;
    virtual bool close() override;
};
}  // namespace jepsen


#endif