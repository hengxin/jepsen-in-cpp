#ifndef WORKER_H
#define WORKER_H

#include "Client.h"
#include "Nemesis.h"
#include "Operation.h"


namespace jepsen {
class Worker {
public:
    Worker() = delete;
    Worker(int id) : id(id) {}
    virtual bool open(int id) = 0;
    virtual bool invoke(Operation& op) = 0;
    virtual bool close() = 0;

    inline int getId() {
        return id;
    }

protected:
    unique_ptr<Client> client;
    int id;
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