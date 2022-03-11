#ifndef CLIENT_H
#define CLIENT_H

#include "Config.h"
#include "Operation.h"
#include "include.h"

class Client {
public:
    virtual bool open(string node) {
        return true;
    }
    virtual bool close() {
        return true;
    }
    virtual bool setup() {
        return true;
    }
    virtual bool invoke(Operation op) {
        return true;
    }
    virtual bool teardown() {
        return true;
    }
};

#endif