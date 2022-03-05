#ifndef WORKER_H
#define WORKER_H

#include "include.h"
#include "Operation.h"

class Worker {
    virtual bool open(int id);
    virtual bool invoke(Operation& op);
    virtual bool close();
};

class ClientWorker: public Worker {

};

class NemesisWorker: public Worker {

};



#endif