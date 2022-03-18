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
    Worker(int id) : id(id), in(std::make_shared<OperationQueue>(OperationQueue(1))) {}
    virtual bool open(int id) = 0;
    virtual bool invoke(Operation& op) = 0;
    virtual bool close() = 0;

    inline int getId() {
        return id;
    }

protected:
    int id;
    unique_ptr<Client> client;
    OperationQueuePtr in;
    std::future<Operation> fut;
};

class ClientWorker : public Worker {
public:
    ClientWorker() = delete;
    ClientWorker(int id, string node) : Worker(id), node(node) {
        this->client = Client::createOne();
    }
    virtual bool open(int id) override;
    virtual bool invoke(Operation& op) override;
    virtual bool close() override;

private:
    string node;
};

class NemesisWorker : public Worker {
public:
    NemesisWorker() = delete;
    NemesisWorker(int id) : Worker(id) {
        this->client = Nemesis::createOne();
    }
    virtual bool open(int id) override;
    virtual bool invoke(Operation& op) override;
    virtual bool close() override;
};
}  // namespace jepsen


#endif